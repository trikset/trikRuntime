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
#include <QtCore/QSharedPointer>

#include "cameraManager.h"
#include "colorSensor.h"
#include "colorSensorInterface.h"
#include "deviceState.h"
#include "lineSensorInterface.h"
#include "objectSensorInterface.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class LineSensor;
class ObjectSensor;
class VideoPreviewSink;

/// Owns the CPU-backed video sensors and their camera lifetime.
///
/// A sensor runs entirely on the ARM CPU and pulls frames from CameraManager.
/// The manager only creates/destroys sensors and forwards their stopped()
/// notifications to the GUI.
class VideoSensorManager : public QObject
{
	Q_OBJECT

public:
	explicit VideoSensorManager(const trikKernel::Configurer &configurer,
		trikHal::HardwareAbstractionInterface &hardwareAbstraction,
		const QSharedPointer<CameraManager> &cameraManager, std::shared_ptr<VideoPreviewSink> previewSink);
	~VideoSensorManager() override;

	ColorSensorInterface *colorSensor(const QString &port);
	LineSensorInterface *lineSensor(const QString &port);
	ObjectSensorInterface *objectSensor(const QString &port);

	/// Stop every active sensor.
	void stop();

	/// Stop and delete every sensor.
	void clear();

	/// Create (or recreate) the sensor of @p deviceClass on @p port.
	void create(const QString &port, const QString &deviceClass);

	/// Stop the sensor holding @p port, if any.
	void releasePort(const QString &port);

	static bool isVideoSensor(const QString &deviceClass);

private:
	void createSensor(const QString &port, const QString &deviceClass);
	bool checkManagerState(const QString &message) const;

	const trikKernel::Configurer &mConfigurer;
	trikHal::HardwareAbstractionInterface &mHardwareAbstractionInterface;
	QSharedPointer<CameraManager> mCameraManager;
	std::shared_ptr<VideoPreviewSink> mPreviewSink;

	DeviceState mState;

	QHash<int, ColorSensor *> mColorSensors;
	QHash<int, LineSensor *> mLineSensors;
	QHash<int, ObjectSensor *> mObjectSensors;
};

} // namespace trikControl
