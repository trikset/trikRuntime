/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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

#include "v4l2CameraImplementation.h"

#include <QtGui/QImage>
#include <QsLog.h>

using namespace trikControl;

V4l2CameraImplementation::V4l2CameraImplementation(const QString &port
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mHal(hardwareAbstraction)
	, mPort(port)
{
}

QVector<uint8_t> V4l2CameraImplementation::getPhoto()
{
	auto result = mHal.captureV4l2StillImage(mPort, getTempDir());
	if (result.empty())
		return result;

	auto image = QImage(result.data(), 320, 240, QImage::Format_RGB888);
	auto saved = image.save(getTempDir().filePath("photo.jpg"), "JPG");
	if (!saved) {
		QLOG_WARN() << "Failed to save captured image";
	}

	return result;
}
