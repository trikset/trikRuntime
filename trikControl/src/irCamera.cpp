/* Copyright 2023 Nick Ponomarev, Vladimir Kutuev
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
#include <algorithm>

#include "configurerHelper.h"
#include "exceptions/incorrectDeviceConfigurationException.h"


namespace trikControl {

IrCamera::IrCamera(const QString &port, const trikKernel::Configurer &configurer
				, trikHal::HardwareAbstractionInterface &hardwareAbstraction) : mState("IR Camera")
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

	mImage.resize(IMAGE_SIZE);
	mImageBuffer.resize(IMAGE_SIZE);
	mSensorData.resize(mSensorHeight * mSensorWidth);
	mSensorDataBuffer.resize(mSensorHeight * mSensorWidth);
	mSensorProcessBuffer.resize(mSensorHeight * mSensorWidth);

	mIrCameraWorker.reset(new IrCameraWorkerMLX90640(i2cAddr));
	mIrCameraWorker->moveToThread(&mWorkerThread);

	connect(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::newFrame, this, &IrCamera::onNewFrame);
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
	QMetaObject::invokeMethod(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::init);
}

void IrCamera::stop()
{
	QMetaObject::invokeMethod(mIrCameraWorker.data(), &IrCameraWorkerMLX90640::stop);
}

QVector<int32_t> IrCamera::getImage()
{
	if (mState.isFailed()) {
		return QVector<int32_t>();
	}

	return mImage;
}

int8_t IrCamera::readSensor(int m, int n)
{
	if(m > mSensorHeight || n > mSensorWidth || m <= 0 || n <= 0) {
		QLOG_WARN() << QString("Incorrect parameters for IrCamera::readSensor: m = %1, n = %2").arg(m).arg(n);
		return -1;
	}

	return mSensorData[m * mSensorWidth + n];
}

void IrCamera::onNewFrame(QVector<uint8_t> frame)
{
	updateImage(frame);
	updateSensor(frame);
	emit newFrame();
}

void IrCamera::onStop()
{
	emit stopped();
}

void IrCamera::updateImage(const QVector<uint8_t> &frame)
{
	std::transform(frame.cbegin(), frame.cend()
		, mImageBuffer.begin()
		, [](uint16_t pixel) {
			int color;
			if (pixel < 64) {
				color = ((pixel * 4) << 16) | 0xFF;
			} else if (pixel < 128) {
				color = 0xFF0000 | (0xFF - ((pixel - 64) * 4));
			} else if (pixel < 192) {
				color = 0xFF0000 | ((pixel - 128) * 4) << 8;
			} else {
				color = (0xFF - ((pixel - 192) * 4)) << 16 | 0xFF00;
			}
			return static_cast<int32_t>(color);
		}
	);
	mImage.swap(mImageBuffer);
}

void IrCamera::updateSensor(const QVector<uint8_t> &frame)
{
	// Average

	for (auto chunkI = 0u, i = 0u; chunkI < mSensorHeight; ++chunkI) {
		auto nextChunkIStart = i + (IMAGE_HEIGHT - i) / (mSensorHeight - chunkI);
		nextChunkIStart += ((IMAGE_HEIGHT - i) % (mSensorHeight - chunkI)) ? 1 : 0;

		for (; i < nextChunkIStart; ++i) {
			for (auto chunkJ = 0u, j = 0u; chunkJ < mSensorWidth; ++chunkJ) {
				auto nextChunkJStart = j + (IMAGE_WIDTH - j) / (mSensorWidth - chunkJ);
				nextChunkJStart += ((IMAGE_WIDTH - j) % (mSensorWidth - chunkJ)) ? 1 : 0;

				for (; j < nextChunkJStart; ++j) {
					mSensorProcessBuffer[chunkI * mSensorWidth + chunkJ] += frame[i * IMAGE_WIDTH + j];
				}
			}
		}
	}

	for (auto i = 0u; i < mSensorHeight; ++i) {
		for (auto j = 0u; j < mSensorWidth; ++j) {
			auto hCount = (IMAGE_HEIGHT / mSensorHeight);
			hCount += (i < IMAGE_HEIGHT % mSensorHeight) ? 1 : 0;
			auto wCount = (IMAGE_WIDTH / mSensorWidth);
			wCount += (j < IMAGE_WIDTH % mSensorWidth) ? 1 : 0;
			auto tempr = (mSensorProcessBuffer[i * mSensorWidth + j] / (hCount * wCount)) / 51;
			mSensorDataBuffer[i * mSensorWidth + j] = tempr & 0x07;
			mSensorProcessBuffer[i * mSensorWidth + j] = 0;
		}
	}
	mSensorData.swap(mSensorDataBuffer);
}

IrCamera::Status IrCamera::status() const
{
	return mState.status();
}

}
