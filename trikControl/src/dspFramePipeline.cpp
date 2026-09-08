/* Copyright 2026 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "dspFramePipeline.h"

#include <QtCore/QThread>
#include <QsLog.h>

#include <trikDsp/dspServer.h>

#include "cameraManager.h"

using namespace trikControl;

DspFramePipeline::DspFramePipeline(const QSharedPointer<CameraManager> &camera, trikDsp::DspServer *dsp)
	: mCamera(camera)
	, mDsp(dsp)
{
}

void DspFramePipeline::setChannel(const ChannelSpec &spec)
{
	{
		std::lock_guard<std::mutex> lock(mChannelLock);
		mChannel = spec;
	}
	mChannelCond.notify_all();
	mCamera->abortFrameWaits(&mChannelChanged);
}

void DspFramePipeline::stop()
{
	{
		std::lock_guard<std::mutex> lock(mChannelLock);
		mRunning = false;
	}
	mChannelCond.notify_all();
	mCamera->abortFrameWaits(&mChannelChanged);
}

void DspFramePipeline::run()
{
	QLOG_INFO() << "DspFramePipeline: loop started on" << QThread::currentThread();

	for (;;) {
		int portId = -1;
		{
			std::unique_lock<std::mutex> lock(mChannelLock);
			if (!mRunning) {
				break;
			}
			portId = mChannel.portId;
		}

		if (portId < 0) {
			// No active channel: sleep until the manager activates us.
			std::unique_lock<std::mutex> lock(mChannelLock);
			mChannelCond.wait(lock, [this] { return !mRunning || mChannel.portId >= 0; });
			continue;
		}

		if (portId != mActivePortId) {
			mActivePortId = portId;
			mSeenSeq = 0;
			if (mVideoOpen) {
				mDsp->setVideoOutput(false);
				mVideoOpen = false;
			}
		}

		// Block until a NEWER frame of this port arrives, or a channel
		// change/stop aborts the wait (via abortFrameWaits).
		mChannelChanged.store(false, std::memory_order_release);
		const Frame frame = mCamera->getFrame(portId, mSeenSeq, -1, &mChannelChanged);
		if (!frame.isValid()) {
			continue; // aborted (channel changed / stop): re-read the channel
		}
		mSeenSeq = frame.sequence();

		// Take the CURRENT channel now: a re-activation (e.g. detect() updated
		// the params) or a switch may have happened while we waited for the frame.
		ChannelSpec spec;
		{
			std::unique_lock<std::mutex> lock(mChannelLock);
			if (!mRunning) {
				break;
			}
			spec = mChannel;
		}
		if (spec.portId != portId) {
			continue; // channel switched while waiting; the frame is returned
		}

		if (spec.videoOut != mVideoOpen) {
			mDsp->setVideoOutput(spec.videoOut);
			mVideoOpen = spec.videoOut;
		}

		// Build the channel from live camera info: the camera is guaranteed to
		// be open (a frame arrived), so format/lineLength are valid.
		const auto info = mCamera->info(portId);
		trikDsp::DspChannel channel;
		channel.sourceId = spec.port;
		channel.algorithm = spec.algorithm;
		channel.inArgs = spec.inArgs;
		channel.videoOut = spec.videoOut;
		channel.width = info.width;
		channel.height = info.height;
		channel.format = info.format;
		channel.lineLength = info.lineLength;
		channel.inputBufferBase = info.inputRegion * info.inputBuffersPerRegion;

		trikDsp::OutArgs out;
		const bool ok = mDsp->processFrame(channel, out, frame.bufferIndex());
#ifdef TRIK_DEBUG_FPS
		mProcessFps.tick();
#endif

		if (ok) {
			// autoDetect is one-shot: consume it after exactly one frame, like
			// the old runtime cleared the command flag right after reading it.
			if (spec.inArgs.autoDetect) {
				out.autoDetect = true;
				{
					std::lock_guard<std::mutex> lock(mChannelLock);
					mChannel.inArgs.autoDetect = false;
				}
			}
			// The manager routes the result by algorithm (Line/Object/Mxn/Jpeg).
			emit sensorResult(spec.portId, spec.algorithm, out);
		}
		// The frame handle drops here: its capture buffer is returned to the
		// driver automatically once nobody references it anymore.
	}

	if (mVideoOpen) {
		mDsp->setVideoOutput(false);
		mVideoOpen = false;
	}
	QLOG_INFO() << "DspFramePipeline: loop stopped";
}
