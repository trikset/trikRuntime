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

#include "videoSensorManager.h"

#include <memory>

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include <trikHal/VideoDeviceFileInterface.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikDsp/dspTypes.h>

#include "configurerHelper.h"
#include "dspSensorBase.h"

namespace trikControl {

namespace {
constexpr uint16_t DSP_RPROC_ID = 1;
}

// JPEG specialization: the encoded bytes (OutArgs::jpegData) are only valid
// inside the DSP processFrame() call, so the encoder must consume the result
// synchronously on the DSP thread - a DirectConnection runs the slot in the
// emitting thread (no copy, no queued marshalling). Line/Object/Mxn keep the
// queued delivery from the primary template.
template<>
void VideoSensorManager::subscribeToResults<JpegEncoderSensor>(JpegEncoderSensor *sensor, int portId,
	trikDsp::Algorithm algorithm)
{
	connect(mPipeline.data(), &DspFramePipeline::sensorResult, sensor,
		[sensor, portId, algorithm](int resultPortId, trikDsp::Algorithm resultAlgorithm,
			const trikDsp::OutArgs &result) {
		if (resultPortId == portId && resultAlgorithm == algorithm) {
			sensor->onResult(result);
		}
	},
		Qt::DirectConnection);
}

VideoSensorManager::VideoSensorManager(const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction,
	const QSharedPointer<CameraManager> &cameraManager)
	: mConfigurer(configurer)
	, mHardwareAbstractionInterface(hardwareAbstraction)
	, mCameraManager(cameraManager)
	, mState("VideoSensorManager")
{
	mDsp.reset(new trikDsp::DspServer(DSP_RPROC_ID));
	mDsp->setFbOutput(mHardwareAbstractionInterface.createFbOutput());

	connect(mDsp.data(), &trikDsp::DspServer::errorOccurred, this, [this](const QString &message) {
		QLOG_ERROR() << "The VideoSensorManager constructor terminated with an error:" << message;
		mState.fail();
	});
	connect(mDsp.data(), &trikDsp::DspServer::successfullyInited, this, [this]() { mState.ready(); });

	// init() is intentionally synchronous here: it starts the IPC stack and
	// mmaps the DSP output buffer, which every subsequent processFrame() depends
	// on. It spins a local QEventLoop, so the UI stays responsive during the
	// (worst-case 15s) wait.
	mDsp->init();
	if (!mState.isReady()) {
		return;
	}

	mPipeline.reset(new DspFramePipeline(mCameraManager, mDsp.data()));

	// The pipeline loop runs directly in the DSP thread (started signal, direct
	// connection). It does not need a Qt event loop: all control goes through
	// the pipeline's mutex-guarded spec.
	mDspThread.reset(new QThread);
	mDspThread->setObjectName(QStringLiteral("DspServer"));
	connect(mDspThread.data(), &QThread::started, mPipeline.data(), &DspFramePipeline::run, Qt::DirectConnection);
	mDspThread->start();
}

void VideoSensorManager::destroyDsp()
{
	if (mPipeline) {
		mPipeline->stop();
	}
	if (mDspThread) {
		// The loop notices mRunning=false within one frame wait and returns;
		// the queued quit() is then processed by the thread's exec().
		mDspThread->quit();
		mDspThread->wait();
	}
	mPipeline.reset();
	mDsp.reset();
	mDspThread.reset();
}

VideoSensorManager::~VideoSensorManager()
{
	destroyDsp();

	for (int portId : mHeldPorts) {
		mCameraManager->release(portId);
	}
	mHeldPorts.clear();

	qDeleteAll(mLineSensors);
	mLineSensors.clear();
	qDeleteAll(mColorSensors);
	mColorSensors.clear();
	qDeleteAll(mObjectSensors);
	mObjectSensors.clear();
	qDeleteAll(mJpegEncoders);
	mJpegEncoders.clear();
}

bool VideoSensorManager::checkManagerState(const QString &message) const
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "VideoSensorManager: attempt to " << message << " on uninitialized";
		return false;
	}
	return true;
}

void VideoSensorManager::createSensor(const QString &port, const QString &deviceClass)
{
	const int portId = mCameraManager->portId(port);
	if (portId < 0) {
		QLOG_ERROR() << "VideoSensorManager: unknown port" << port;
		return;
	}

	if (deviceClass == QStringLiteral("lineSensor")) {
		if (mLineSensors.contains(portId)) {
			return;
		}
		auto *s = new LineSensor(port, mConfigurer);
		const auto algorithm = s->isEdge() ? trikDsp::Algorithm::EdgeLine : trikDsp::Algorithm::Line;
		connect(s, &LineSensor::activateRequested, this,
			[this, port, algorithm](const trikDsp::InArgs &args, bool videoOut, bool canOpen) {
			activateForPort(port, algorithm, args, videoOut, canOpen);
		}, Qt::QueuedConnection);
		connect(s, &LineSensor::stopRequested, this,
			[this, port](int flags) { handleStopRequested(port, flags); }, Qt::QueuedConnection);
		subscribeToResults(s, portId, algorithm);
		mLineSensors.insert(portId, s);
	} else if (deviceClass == QStringLiteral("objectSensor")) {
		if (mObjectSensors.contains(portId)) {
			return;
		}
		auto *s = new ObjectSensor(port, mConfigurer);
		connect(s, &ObjectSensor::activateRequested, this,
			[this, port](const trikDsp::InArgs &args, bool videoOut, bool canOpen) {
			activateForPort(port, trikDsp::Algorithm::Object, args, videoOut, canOpen);
		}, Qt::QueuedConnection);
		connect(s, &ObjectSensor::stopRequested, this,
			[this, port](int flags) { handleStopRequested(port, flags); }, Qt::QueuedConnection);
		subscribeToResults(s, portId, trikDsp::Algorithm::Object);
		mObjectSensors.insert(portId, s);
	} else if (deviceClass == QStringLiteral("colorSensor")) {
		if (mColorSensors.contains(portId)) {
			return;
		}
		auto *s = new ColorSensor(port, mConfigurer);
		connect(s, &ColorSensor::activateRequested, this,
			[this, port](const trikDsp::InArgs &args, bool videoOut, bool canOpen) {
			activateForPort(port, trikDsp::Algorithm::Mxn, args, videoOut, canOpen);
		}, Qt::QueuedConnection);
		connect(s, &ColorSensor::stopRequested, this,
			[this, port](int flags) { handleStopRequested(port, flags); }, Qt::QueuedConnection);
		subscribeToResults(s, portId, trikDsp::Algorithm::Mxn);
		mColorSensors.insert(portId, s);
	} else if (deviceClass == QStringLiteral("jpegEncoderSensor")) {
		if (mJpegEncoders.contains(portId)) {
			return;
		}
		auto *s = new JpegEncoderSensor(port, mConfigurer, mHardwareAbstractionInterface);
		connect(s, &JpegEncoderSensor::activateRequested, this,
			[this, port](const trikDsp::InArgs &args, bool videoOut, bool canOpen) {
			activateForPort(port, trikDsp::Algorithm::Jpeg, args, videoOut, canOpen);
		}, Qt::QueuedConnection);
		connect(s, &JpegEncoderSensor::stopRequested, this,
			[this, port](int flags) { handleStopRequested(port, flags); }, Qt::QueuedConnection);
		subscribeToResults(s, portId, trikDsp::Algorithm::Jpeg);
		mJpegEncoders.insert(portId, s);
	}
}

void VideoSensorManager::activateForPort(const QString &port, trikDsp::Algorithm algo, const trikDsp::InArgs &args,
	bool videoOut, bool canOpen)
{
	const int portId = mCameraManager->portId(port);
	if (portId < 0) {
		QLOG_ERROR() << "VideoSensorManager: unknown port" << port;
		return;
	}

	if (!mHeldPorts.contains(portId)) {
		// The camera is not held: start the (async) acquisition and request the
		// DSP channel right away. The pipeline simply pulls empty until frames
		// start flowing once the camera is open, so no pending-activation
		// bookkeeping is needed.
		if (!canOpen) {
			return;
		}
		mCameraManager->acquire(portId);
		mHeldPorts.insert(portId);
	}

	activateDsp(portId, algo, args, videoOut);
}

void VideoSensorManager::activateDsp(int portId, trikDsp::Algorithm algo, const trikDsp::InArgs &args, bool videoOut)
{
	DspFramePipeline::ChannelSpec spec;
	spec.portId = portId;
	spec.port = mCameraManager->portName(portId);
	spec.algorithm = algo;
	spec.inArgs = args;
	spec.videoOut = videoOut;

	mPipeline->setChannel(spec);
	mActivePortId = portId;
	QLOG_INFO() << "VideoSensorManager: DSP activated on port" << spec.port << "algo" << static_cast<int>(algo);
}

void VideoSensorManager::handleStopRequested(const QString &port, int flags)
{
	const int portId = mCameraManager->portId(port);

	// The DSP is single-channel: deactivate it only when the port being stopped
	// actually owns the active channel.
	if (mActivePortId == portId) {
		DspFramePipeline::ChannelSpec spec;
		mPipeline->setChannel(spec);
		mActivePortId = -1;
	}

	if (flags & StopStream) {
		// Park the camera: latch the stop so a later acquire/release parks it
		// again, keep the device open for a quick re-acquire.
		mCameraManager->stopStreaming(port);
		if (mHeldPorts.remove(portId)) {
			mCameraManager->release(portId);
		}
	} else if (flags & StopAll) {
		if (mHeldPorts.remove(portId)) {
			mCameraManager->release(portId);
		}
	}
	// StopNone: only the DSP is deactivated; the camera keeps streaming.

	if (flags & (StopStream | StopAll)) {
		Q_EMIT sensorStopped();
	}
}

void VideoSensorManager::create(const QString &port, const QString &deviceClass)
{
	if (!checkManagerState("create"))
		return;
	createSensor(port, deviceClass);
}

void VideoSensorManager::stop()
{
	QLOG_INFO() << "VideoSensorManager::stop: heldPorts=" << mHeldPorts.size()
		    << "activeDspPortId=" << mActivePortId;
	if (!checkManagerState("stop"))
		return;

	// The DSP is single-channel: if the detached encoder is the active channel
	// it must keep streaming, so skip the deactivation.
	if (mActivePortId >= 0 && !mDetachedPorts.contains(mActivePortId)) {
		DspFramePipeline::ChannelSpec spec;
		mPipeline->setChannel(spec);
		mActivePortId = -1;
		QLOG_INFO() << "VideoSensorManager::stop: DSP deactivated";
	}

	// Stop and release every held camera except detached ports.
	for (auto it = mHeldPorts.begin(); it != mHeldPorts.end();) {
		if (mDetachedPorts.contains(*it)) {
			++it;
			continue;
		}
		mCameraManager->release(*it);
		it = mHeldPorts.erase(it);
	}
	QLOG_INFO() << "VideoSensorManager::stop: held ports released";
}

void VideoSensorManager::releasePort(const QString &port)
{
	const int portId = mCameraManager->portId(port);

	if (mActivePortId == portId) {
		DspFramePipeline::ChannelSpec spec;
		mPipeline->setChannel(spec);
		mActivePortId = -1;
	}
	mDetachedPorts.remove(portId);

	if (mHeldPorts.remove(portId)) {
		mCameraManager->release(portId);
	}
}

void VideoSensorManager::setPortDetached(const QString &port, bool detached)
{
	const int portId = mCameraManager->portId(port);
	if (detached) {
		mDetachedPorts.insert(portId);
	} else {
		mDetachedPorts.remove(portId);
	}
}

void VideoSensorManager::stopTranslation(const QString &port, bool keepCamera)
{
	const int portId = mCameraManager->portId(port);
	mDetachedPorts.remove(portId);

	auto it = mJpegEncoders.find(portId);
	if (it == mJpegEncoders.end()) {
		return;
	}

	it.value()->stop(keepCamera ? StopStream : StopAll);
}

void VideoSensorManager::clear()
{
	const auto eraseNonDetached = [this](auto &map) {
		for (auto it = map.begin(); it != map.end();) {
			if (mDetachedPorts.contains(it.key())) {
				++it;
			} else {
				delete it.value();
				it = map.erase(it);
			}
		}
	};
	eraseNonDetached(mLineSensors);
	eraseNonDetached(mColorSensors);
	eraseNonDetached(mObjectSensors);
	eraseNonDetached(mJpegEncoders);
}

bool VideoSensorManager::isVideoSensor(const QString &deviceClass)
{
	return deviceClass == QStringLiteral("lineSensor") || deviceClass == QStringLiteral("objectSensor")
	       || deviceClass == QStringLiteral("colorSensor");
}

LineSensorInterface *VideoSensorManager::lineSensor(const QString &port)
{
	const int id = mCameraManager->portId(port);
	auto it = mLineSensors.find(id);
	return it == mLineSensors.end() ? nullptr : *it;
}
ColorSensorInterface *VideoSensorManager::colorSensor(const QString &port)
{
	const int id = mCameraManager->portId(port);
	auto it = mColorSensors.find(id);
	return it == mColorSensors.end() ? nullptr : *it;
}
ObjectSensorInterface *VideoSensorManager::objectSensor(const QString &port)
{
	const int id = mCameraManager->portId(port);
	auto it = mObjectSensors.find(id);
	return it == mObjectSensors.end() ? nullptr : *it;
}

JpegEncoderSensor *VideoSensorManager::jpegEncoderSensor(const QString &port)
{
	if (!checkManagerState("jpegEncoderSensor"))
		return nullptr;
	createSensor(port, QStringLiteral("jpegEncoderSensor"));
	const int id = mCameraManager->portId(port);
	auto it = mJpegEncoders.find(id);
	return it == mJpegEncoders.end() ? nullptr : *it;
}

}
