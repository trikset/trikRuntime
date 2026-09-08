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

#include <QtCore/QHash>
#include <QtCore/QScopedPointer>
#include <QtCore/QSet>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QThread>

#include <trikDsp/dspServer.h>

#include "lineSensorInterface.h"
#include "objectSensorInterface.h"
#include "colorSensorInterface.h"
#include "lineSensor.h"
#include "objectSensor.h"
#include "colorSensor.h"
#include "jpegEncoderSensor.h"
#include "deviceState.h"
#include "cameraManager.h"
#include "dspFramePipeline.h"

#include <trikControl/videoSensorStopFlags.h>

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikKernel {
class Configurer;
}

namespace trikControl {

/// Controller for DSP-based video sensors across multiple camera ports.
///
/// Owns one DspServer (DSP bridge), one DspFramePipeline (the pull-based frame
/// consumer running on the DSP thread) and per-port sensor instances. Camera
/// lifecycle is delegated to CameraManager (pull-based, no frame signals).
///
/// ## Threading
///
/// VSM lives on the GUI/main thread. The DspFramePipeline runs on mDspThread
/// (a plain loop, not a Qt event loop): it blocks on the CameraManager until a
/// new frame arrives, runs one blocking DSP step and emits a cheap
/// sensorResult(portId, algorithm, result) signal. The manager routes the
/// result to the right sensor by algorithm (Line/Object/Mxn/Jpeg); the JPEG
/// encoder writes its FIFO from the GUI thread.
///
/// Control is mutex/condvar-based on the pipeline (no Qt events): activate/
/// deactivate take effect promptly by aborting the pipeline's frame wait.
class VideoSensorManager : public QObject
{
	Q_OBJECT

public:
	explicit VideoSensorManager(const trikKernel::Configurer &configurer,
		const trikHal::HardwareAbstractionInterface &hardwareAbstraction,
		const QSharedPointer<CameraManager> &cameraManager);
	~VideoSensorManager() override;

	LineSensorInterface *lineSensor(const QString &port);
	ObjectSensorInterface *objectSensor(const QString &port);
	ColorSensorInterface *colorSensor(const QString &port);

	/// Internal (non-Brick) JPEG encoder sensor. Creates it on first use.
	JpegEncoderSensor *jpegEncoderSensor(const QString &port);

	/// Full teardown: deactivate the DSP and stop + release every held camera
	/// except detached ports.
	void stop();
	void clear();
	void create(const QString &port, const QString &deviceClass);

	/// Forcibly release our hold on @p port: deactivate the DSP, drop the push
	/// subscription and release the camera refcount, so the port is fully free.
	void releasePort(const QString &port);

	/// Mark @p port as detached: its sensor/encoder and camera are kept alive
	/// across stop()/clear() until explicitly stopped via stopVideoTranslation().
	void setPortDetached(const QString &port, bool detached);

	/// Stop the JPEG encoder translation on @p port (if any) and clear its
	/// detached flag. If @p keepCamera is true the camera is only streamed off
	/// (kept acquired) so a subsequent sensor init switches the DSP algorithm
	/// without reopening the device; otherwise the camera is released too.
	void stopTranslation(const QString &port, bool keepCamera);

	static bool isVideoSensor(const QString &deviceClass);

Q_SIGNALS:
	/// Emitted when a video sensor stops in a way that tears the camera down
	/// (StopStream or StopAll), so a stale frame may be left on the framebuffer.
	void sensorStopped();

private:
	void activateForPort(const QString &port, trikDsp::Algorithm algo, const trikDsp::InArgs &args, bool videoOut,
		bool canOpen);
	void activateDsp(int portId, trikDsp::Algorithm algo, const trikDsp::InArgs &args, bool videoOut);
	void handleStopRequested(const QString &port, int flags);
	void createSensor(const QString &port, const QString &deviceClass);
	bool checkManagerState(const QString &message) const;
	void destroyDsp();

	/// Subscribe @p sensor to the pipeline's sensorResult() for exactly
	/// (portId, algorithm). Each sensor consumes its own results - the pipeline
	/// never knows about sensors, and the manager does no per-frame routing.
	template<typename Sensor>
	void subscribeToResults(Sensor *sensor, int portId, trikDsp::Algorithm algorithm)
	{
		connect(mPipeline.data(), &DspFramePipeline::sensorResult, sensor,
			[sensor, portId, algorithm](int resultPortId, trikDsp::Algorithm resultAlgorithm,
				const trikDsp::OutArgs &result) {
			if (resultPortId == portId && resultAlgorithm == algorithm) {
				sensor->onResult(result);
			}
		},
			Qt::QueuedConnection);
	}

	const trikKernel::Configurer &mConfigurer;
	const trikHal::HardwareAbstractionInterface &mHardwareAbstractionInterface;
	QSharedPointer<CameraManager> mCameraManager;

	DeviceState mState;

	QScopedPointer<trikDsp::DspServer> mDsp;
	QScopedPointer<DspFramePipeline> mPipeline;
	QScopedPointer<QThread> mDspThread;

	/// Port ids whose cameras this manager currently holds.
	QSet<int> mHeldPorts;

	/// Id of the port currently driving the DSP channel (-1 when inactive).
	int mActivePortId = -1;

	/// Ports whose translation/sensor is detached and must survive stop()/clear().
	QSet<int> mDetachedPorts;

	/// Sensor instances indexed by port id.
	QHash<int, LineSensor *> mLineSensors;
	QHash<int, ColorSensor *> mColorSensors;
	QHash<int, ObjectSensor *> mObjectSensors;
	QHash<int, JpegEncoderSensor *> mJpegEncoders;
};

}
