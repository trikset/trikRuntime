/* Copyright 2023 Nick Ponomarev
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

#include "irCamera.h"

// #include <QtCore/QSysInfo>

#include <QsLog.h>
#include <trikKernel/configurer.h>

// #include <QEventLoop>
// #include <QThread>
// #include <functional>

#include <QVector>
#include <algorithm>


#include "configurerHelper.h"

namespace trikControl {

IrCamera::IrCamera(const QString &port, const trikKernel::Configurer &configurer
				, trikHal::HardwareAbstractionInterface &hardwareAbstraction) : mState("IR Camera")
{
	Q_UNUSED(hardwareAbstraction)

	mI2cAddr = ConfigurerHelper::configureInt(configurer, mState, port, "i2cAddress");
	QLOG_INFO() << "Trying to init IR camera on address " << mI2cAddr;

	// Library written primary in C style, so no exceptions, only error codes
	int status;

	// Default value is 4 Hz, greater values may require more i2c speed
	status = MLX90640_SetRefreshRate(mI2cAddr, mRefreshRate);
	if (status != 0) {
		QString info = status == -1 ? "Got NACK." : "Written incorrect value";
		QLOG_ERROR() << "Failed to set IR camera refresh rate. " << info;
		mState.fail();
	}

	// Ensure that camera works in recommended mode
	status = MLX90640_SetChessMode(mI2cAddr);
	if (status != 0) {
		QString info = status == -1 ? "Got NACK." : "Written incorrect value";
		QLOG_ERROR() << "Failed to set IR camera mode to chess. " << info;
		mState.fail();
	}

	QVector<uint16_t> eeprom(EEPROM_SIZE);
	status = MLX90640_DumpEE(mI2cAddr, eeprom.data());
	if (status == -1) {
		QLOG_ERROR() << "Failed to dump IR camera EEPROM. Got NACK.";
		mState.fail();
	}

	status = MLX90640_ExtractParameters(eeprom.data(), &mParams);
	if (status == -7) {
		QLOG_ERROR() << "Failed extract parameters from IR camerea EEPROM.";
		mState.fail();
	}

	mState.ready();
	QLOG_INFO() << "Initialized IR camera";
}


QVector<int32_t> IrCamera::getImage() {

//	QMutexLocker lock(&mCameraMutex);
//	QVector<uint8_t> photo;
//	std::function<void()> runFunc = [this, &photo](){ mCameraImpl->getPhoto().swap(photo); };
// #if QT_VERSION_MAJOR>=5 && QT_VERSION_MINOR>=10 && QT_VERSION_PATCH >= 2
//	QScopedPointer<QThread> t(QThread::create(std::move(runFunc)));
// #else
//	struct CameraThread: public QThread {
//		explicit CameraThread(std::function<void()> &&f): mF(f) {}
//		void run() override { mF(); }
//		std::function<void()> mF;
//	};
//	QScopedPointer<QThread> t(new CameraThread(std::move(runFunc)));
// #endif
//	QEventLoop l;
//	QObject::connect(t.data(), &QThread::finished, &l, &QEventLoop::quit);
//	t->setObjectName(__PRETTY_FUNCTION__);
//	t->start();
//	l.exec();
//	return photo;

	if (mState.isFailed()) {
		return QVector<int32_t>();
	}

	QVector<uint16_t> frame(FRAME_SIZE);
	int status = MLX90640_GetFrameData(mI2cAddr, frame.data());
	if (status != 0) {
		QString info = status == -1 ? "Got NACK." : "Frame is corrupted or can't acquire it.";
		QLOG_ERROR() << "Failed to get frame from IR. " << info;
		return QVector<int32_t>();
	}

	QVector<int32_t> image(IMAGE_SIZE);
	std::transform(frame.cbegin(), frame.cend() - (FRAME_SIZE - IMAGE_SIZE), image.begin(),
				   [](uint16_t pixel){ return (pixel & 0xff); });

	return image;
}


IrCamera::Status IrCamera::status() const {
	return mState.status();
}
}
