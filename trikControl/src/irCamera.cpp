/* Copyright 2023-2024 Nick Ponomarev, Vladimir Kutuev
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

#include <QsLog.h>
#include <trikKernel/configurer.h>

#include "configurerHelper.h"
#include "exceptions/incorrectDeviceConfigurationException.h"


namespace trikControl {

IrCamera::IrCamera(const QString &port, const trikKernel::Configurer &configurer
				, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mImage(IMAGE_SIZE), mState("IR Camera")
{
	Q_UNUSED(hardwareAbstraction)

	uint8_t i2cAddr = ConfigurerHelper::configureInt(configurer, mState, port, "i2cAddress");
	QLOG_INFO() << "Trying to init IR camera on address " << i2cAddr;

	const int m = ConfigurerHelper::configureInt(configurer, mState, port, "m");
	if (m <= 0) {
		mState.fail();
		throw IncorrectDeviceConfigurationException("Ir Camera shall have 'm' parameter greater than zero");
	}
	mSensorHeight = m;

	const int n = ConfigurerHelper::configureInt(configurer, mState, port, "n");
	if (n <= 0) {
		mState.fail();
		throw IncorrectDeviceConfigurationException("Ir Camera shall have 'n' parameter greater than zero");
	}
	mSensorWidth = n;

	mSensorData.resize(mSensorHeight * mSensorWidth);
	mSensorData.fill(-1);

	mIrCameraWorker.reset(new IrCameraWorkerMLX90640(i2cAddr, m, n));
	mIrCameraWorker->moveToThread(&mWorkerThread);

	connect(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::newImage, this, &IrCamera::onNewImage);
	connect(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::newSensorData, this, &IrCamera::onNewSensorData);
	connect(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::stopped, this, &IrCamera::onStop);

	QLOG_INFO() << "Starting IrCamera worker thread" << &mWorkerThread;

	mWorkerThread.setObjectName(mIrCameraWorker->metaObject()->className());
	mWorkerThread.start();
	mState.ready();
	QLOG_INFO() << "Initialized IR camera";
}

IrCamera::~IrCamera()
{
	mState.stop();
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

void IrCamera::init()
{
	mSensorData.fill(0);
	QMetaObject::invokeMethod(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::init);
}

void IrCamera::stop()
{
	QMetaObject::invokeMethod(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::stop);
}

QVector<int32_t> IrCamera::getImage() const
{
	if (mState.isFailed()) {
		return QVector<int32_t>();
	}

	return mImage;
}

int IrCamera::readSensor(int m, int n) const
{
	if(m > mSensorHeight || n > mSensorWidth || m <= 0 || n <= 0) {
		QLOG_WARN() << QString("Incorrect parameters for IrCamera::readSensor: m = %1, n = %2").arg(m).arg(n);
		return -1;
	}

	return mSensorData[(m - 1) * mSensorWidth + (n - 1)];
}

void IrCamera::onNewImage(QVector<int32_t> image)
{
	mImage.swap(image);
	emit imageUpdated();
}

void IrCamera::onNewSensorData(QVector<int32_t> grid)
{
	mSensorData.swap(grid);
	emit sensorUpdated();
}

void IrCamera::onStop()
{
	mImage.fill(0);
	mSensorData.fill(-1);
	emit stopped();
}

IrCamera::Status IrCamera::status() const
{
	return mState.status();
}

}
