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

#include "cameraDevice.h"

#include <QtCore/QSysInfo>

#include "qtCameraImplementation.h"
#include "v4l2CameraImplementation.h"
#include "imitationCameraImplementation.h"
#include <QsLog.h>
#include <trikKernel/configurer.h>

namespace trikControl {

CameraDevice::CameraDevice(const QString & mediaPath, const trikKernel::Configurer &configurer
							, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
{
	Q_UNUSED(hardwareAbstraction);

	QString type = configurer.attributeByDevice("camera", "type");
	QString src = configurer.attributeByDevice("camera", "src");

	QString osInfo = QSysInfo::productType();
	QString currentArch = QSysInfo::currentCpuArchitecture();
	QString failMessage;

	QLOG_INFO() << "OS name: " << osInfo << " currentArch: " << currentArch;

	bool isLinux = osInfo == "ubuntu" || osInfo == "debian"; // can be done with macros Q_OS_LINUX more easily
	if (type == "qtmultimedia") {
		if (! isLinux || currentArch != "arm") {
			decltype(mCameraImpl)(new QtCameraImplementation(src)).swap(mCameraImpl);
		} else {
			failMessage = "can not use qt camera on TRIK controller";
		}
	} else if (type == "v4l2") {
				if (isLinux) {
					decltype(mCameraImpl)(new V4l2CameraImplementation(src, hardwareAbstraction)).swap(mCameraImpl);
				} else {
					failMessage = "can use v4l2 only on Linux";
				}
	} else if (type == "file") {
				QStringList filters = configurer.attributeByDevice("camera", "filters").split(',');
				decltype(mCameraImpl)(new ImitationCameraImplementation(filters, mediaPath)).swap(mCameraImpl);
	} else {
		failMessage = "unknown camera device type";
	}

	if (mCameraImpl) {
		mCameraImpl->setTempDir(mediaPath);
	} else {
		if (failMessage.isEmpty()) {
			failMessage = "object construction failure";
		}

		QLOG_ERROR() << "Failed to initialize camera device for " << src
				<< ", " << failMessage
				<< ", creating camera with type=file ("
				<< mediaPath << ") and filters=\"*.jpg,*.png\"";
		decltype(mCameraImpl)(new ImitationCameraImplementation(QStringList({"*.jpg","*.png"}), mediaPath))
				.swap(mCameraImpl);
	}
}


QVector<uint8_t> CameraDevice::getPhoto() {
	if (mCameraImpl)
		return mCameraImpl->getPhoto();
	else
		return QVector<uint8_t>();
}


CameraDevice::Status CameraDevice::status() const {
	return CameraDevice::Status::ready;
}
}
