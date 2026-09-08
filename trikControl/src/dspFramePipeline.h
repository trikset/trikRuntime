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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>

#include <trikDsp/dspTypes.h>

#ifdef TRIK_DEBUG_FPS
#	include <trikKernel/fpsCounter.h>
#endif

namespace trikControl {

class CameraManager;
}
namespace trikDsp {
class DspServer;
}

namespace trikControl {

/// Pull-based DSP frame consumer, running on the DSP thread.
///
/// Owns no thread: VideoSensorManager starts it on its DSP worker thread via
/// run(). The loop blocks on the CameraManager until a NEWER frame of the
/// active port arrives, then takes the current channel (a re-activation such as
/// detect() may have updated the parameters while waiting), runs one blocking
/// DSP STEP and emits a cheap sensorResult() signal. The manager routes the
/// result to the right sensor/encoder by the algorithm, like the old onResult().
///
/// Control (setChannel/stop) is driven from the GUI thread through a
/// mutex-guarded spec + condition variable: a channel change aborts the frame
/// wait via CameraManager::abortFrameWaits(), so deactivation/shutdown is
/// immediate and no polling is involved.
class DspFramePipeline : public QObject
{
	Q_OBJECT

public:
	/// The requested DSP channel. The negotiated format is NOT stored here: it
	/// is read from the CameraManager at process time, when the camera is
	/// guaranteed to be open and streaming.
	struct ChannelSpec {
		int portId = -1;
		QString port;
		trikDsp::Algorithm algorithm = trikDsp::Algorithm::None;
		trikDsp::InArgs inArgs = {};
		bool videoOut = false;
	};

	DspFramePipeline(const QSharedPointer<CameraManager> &camera, trikDsp::DspServer *dsp);

	/// Replace the active channel. A spec with portId < 0 deactivates the DSP.
	void setChannel(const ChannelSpec &spec);

	/// Terminate the processing loop. Takes effect promptly: wakes any pending
	/// frame wait.
	void stop();

	/// The processing loop; MUST run on the DSP thread (started by the manager
	/// when the DSP thread starts). Returns only after stop().
	void run();

Q_SIGNALS:
	/// Result of one processed frame, delivered to the GUI thread (queued). The
	/// manager routes it by algorithm (Line/Object/Mxn/Jpeg).
	void sensorResult(int portId, trikDsp::Algorithm algorithm, const trikDsp::OutArgs &result);

private:
	QSharedPointer<CameraManager> mCamera;
	trikDsp::DspServer *mDsp;

	/// Guards mChannel, mRunning.
	std::mutex mChannelLock;
	/// Notified by setChannel()/stop(); the loop waits on it while inactive.
	std::condition_variable mChannelCond;
	ChannelSpec mChannel;
	bool mRunning = true;

	/// Set by CameraManager::abortFrameWaits() on setChannel()/stop(), passed
	/// to getFrame() as the abort flag: it makes a pending frame wait return
	/// immediately so the loop re-reads the channel.
	std::atomic<bool> mChannelChanged {false};

	/// Per-active-port state, touched only by the loop thread.
	uint32_t mSeenSeq = 0;
	int mActivePortId = -1;
	bool mVideoOpen = false;

#ifdef TRIK_DEBUG_FPS
	/// TEMP DEBUG: DSP STEP completion rate (logged every few seconds).
	trikKernel::FpsCounter mProcessFps {QStringLiteral("DspFramePipeline process")};
#endif
};

}
