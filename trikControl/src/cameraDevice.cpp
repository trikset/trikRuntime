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

#include <QEventLoop>
#include <QThread>
#include <functional>

namespace trikControl {

CameraDevice::CameraDevice(const QString & port, const QString & mediaPath, const trikKernel::Configurer &configurer
							, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
{
	Q_UNUSED(hardwareAbstraction)

	QString type = configurer.attributeByPort(port, "type");
	QString src = configurer.attributeByPort(port, "src");

	QString failMessage;

	if (type == "qtmultimedia") {
			decltype(mCameraImpl)(new QtCameraImplementation(src)).swap(mCameraImpl);
	} else if (type == "v4l2") {
#ifdef Q_OS_LINUX
			decltype(mCameraImpl)(new V4l2CameraImplementation(src, hardwareAbstraction)).swap(mCameraImpl);
#else
			failMessage = "can use v4l2 only on Linux";
#endif
	} else if (type == "file") {
				QStringList filters = configurer.attributeByPort(port, "filters").split(',');
				decltype(mCameraImpl)(new ImitationCameraImplementation(filters, mediaPath)).swap(mCameraImpl);
	} else {
		failMessage = "unknown camera device type:" + type;
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
	if (!mCameraImpl) {
		return QVector<uint8_t>();
	}

	QMutexLocker lock(&mCameraMutex);
	QVector<uint8_t> photo;
	std::function<void()> runFunc = [this, &photo](){ mCameraImpl->getPhoto().swap(photo); };
#if QT_VERSION_MAJOR>=5 && QT_VERSION_MINOR>=10 && QT_VERSION_PATCH >= 2
	QScopedPointer<QThread> t(QThread::create(std::move(runFunc)));
#else
	struct CameraThread: public QThread {
		explicit CameraThread(std::function<void()> &&f): mF(f) {}
		void run() override { mF(); }
		std::function<void()> mF;
	};
	QScopedPointer<QThread> t(new CameraThread(std::move(runFunc)));
#endif
	QEventLoop l;
	QObject::connect(t.data(), &QThread::finished, &l, &QEventLoop::quit);
	t->setObjectName(__PRETTY_FUNCTION__);
	t->start();
	l.exec();
	return photo;
}


CameraDevice::Status CameraDevice::status() const {
	return CameraDevice::Status::ready;
}
}
