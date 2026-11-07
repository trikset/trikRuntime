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

#include <utility>

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikControl/videoPreviewSink.h>

#include "lineSensor.h"
#include "objectSensor.h"

namespace trikControl {

VideoSensorManager::VideoSensorManager(const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction, const QSharedPointer<CameraManager> &cameraManager,
	std::shared_ptr<VideoPreviewSink> previewSink)
	: mConfigurer(configurer)
	, mHardwareAbstractionInterface(hardwareAbstraction)
	, mCameraManager(cameraManager)
	, mPreviewSink(std::move(previewSink))
	, mState("VideoSensorManager")
{
	mState.ready();
}

VideoSensorManager::~VideoSensorManager()
{
	qDeleteAll(mColorSensors);
	mColorSensors.clear();
	qDeleteAll(mLineSensors);
	mLineSensors.clear();
	qDeleteAll(mObjectSensors);
	mObjectSensors.clear();
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
	const auto portId = mCameraManager->portId(port);
	if (portId < 0) {
		QLOG_ERROR() << "VideoSensorManager: unknown port" << port;
		return;
	}

	if (deviceClass == QStringLiteral("colorSensor")) {
		if (mColorSensors.contains(portId)) {
			return;
		}

		auto *s =
			new ColorSensor(port, mConfigurer, mCameraManager, mHardwareAbstractionInterface, mPreviewSink);
		mColorSensors.insert(portId, s);
		return;
	}

	if (deviceClass == QStringLiteral("lineSensor")) {
		if (mLineSensors.contains(portId)) {
			return;
		}

		auto *s =
			new LineSensor(port, mConfigurer, mCameraManager, mHardwareAbstractionInterface, mPreviewSink);
		mLineSensors.insert(portId, s);
		return;
	}

	if (deviceClass == QStringLiteral("objectSensor")) {
		if (mObjectSensors.contains(portId)) {
			return;
		}

		auto *s = new ObjectSensor(port, mConfigurer, mCameraManager, mHardwareAbstractionInterface,
			mPreviewSink);
		mObjectSensors.insert(portId, s);
		return;
	}

	QLOG_WARN() << "VideoSensorManager: unsupported video sensor class" << deviceClass << "on" << port;
}

void VideoSensorManager::create(const QString &port, const QString &deviceClass)
{
	if (!checkManagerState("create")) {
		return;
	}
	createSensor(port, deviceClass);
}

void VideoSensorManager::stop()
{
	if (!checkManagerState("stop")) {
		return;
	}

	for (auto *sensor : mColorSensors) {
		sensor->stop();
	}

	for (auto *sensor : mLineSensors) {
		sensor->stop();
	}

	for (auto *sensor : mObjectSensors) {
		sensor->stop();
	}
}

void VideoSensorManager::clear()
{
	qDeleteAll(mColorSensors);
	mColorSensors.clear();
	qDeleteAll(mLineSensors);
	mLineSensors.clear();
	qDeleteAll(mObjectSensors);
	mObjectSensors.clear();
}

void VideoSensorManager::releasePort(const QString &port)
{
	const auto portId = mCameraManager->portId(port);

	const auto colorIt = mColorSensors.find(portId);
	if (colorIt != mColorSensors.end()) {
		(*colorIt)->stop();
	}

	const auto lineIt = mLineSensors.find(portId);
	if (lineIt != mLineSensors.end()) {
		(*lineIt)->stop();
	}

	const auto objectIt = mObjectSensors.find(portId);
	if (objectIt != mObjectSensors.end()) {
		(*objectIt)->stop();
	}
}

bool VideoSensorManager::isVideoSensor(const QString &deviceClass)
{
	return deviceClass == QStringLiteral("colorSensor") || deviceClass == QStringLiteral("lineSensor")
	       || deviceClass == QStringLiteral("objectSensor");
}

ColorSensorInterface *VideoSensorManager::colorSensor(const QString &port)
{
	const auto id = mCameraManager->portId(port);
	const auto it = mColorSensors.find(id);
	return it == mColorSensors.end() ? nullptr : *it;
}

LineSensorInterface *VideoSensorManager::lineSensor(const QString &port)
{
	const auto id = mCameraManager->portId(port);
	const auto it = mLineSensors.find(id);
	return it == mLineSensors.end() ? nullptr : *it;
}

ObjectSensorInterface *VideoSensorManager::objectSensor(const QString &port)
{
	const auto id = mCameraManager->portId(port);
	const auto it = mObjectSensors.find(id);
	return it == mObjectSensors.end() ? nullptr : *it;
}

} // namespace trikControl
