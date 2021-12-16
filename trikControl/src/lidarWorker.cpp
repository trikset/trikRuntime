/* Copyright 2021 CyberTech Labs Ltd.
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

#include "lidarWorker.h"

using namespace trikControl;

constexpr uint8_t PKG_HEADER = 0xAA;
constexpr int SIZE_BYTE = 1;

constexpr uint8_t PROTOCOL_VERSION = 0x01;
constexpr int PROTOCOL_BYTE = 3;

constexpr uint8_t PKG_TYPE = 0x61;
constexpr int TYPE_BYTE = 4;

constexpr uint8_t DATA_HEADER = 0xAD;
constexpr int DATA_HEADER_BYTE = 5;

constexpr int DATA_SIZE_BYTE = 6;
constexpr int START_ANGLE_BYTE = 11;
constexpr int FIRST_DIST_BYTE = 14;

constexpr int ANGLES_RAW_NUMBER = 36000;
constexpr int ANGLE_STEP = ANGLES_RAW_NUMBER / 16;
constexpr int ANGLES_NUMBER = 360;

trikControl::LidarWorker::LidarWorker(const QString &fileName
					, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mFifoFileName(fileName)
	, mHardwareAbstraction(hardwareAbstraction)
	, mResult(ANGLES_RAW_NUMBER, 0)
{
	mWaitForInit.acquire(1);
}

LidarWorker::~LidarWorker()
{
	mFifo.reset();
}

LidarWorker::Status LidarWorker::status() const
{
	return mFifo->status();
}

void LidarWorker::init()
{
	mFifo.reset(new Fifo(mFifoFileName, mHardwareAbstraction));
	connect(mFifo.data(), &Fifo::newData, this, &LidarWorker::onNewData);
	mWaitForInit.release(1);
}

QVector<int> LidarWorker::read() const
{
	QVector<int> result(ANGLES_NUMBER, 0);
	constexpr int meanWindow = ANGLES_RAW_NUMBER / ANGLES_NUMBER; // 100
	constexpr int halfWindow = meanWindow / 2; // 50
	for (auto i = halfWindow; i < mResult.size(); i += meanWindow) {
		result[(i + halfWindow) % ANGLES_RAW_NUMBER / meanWindow] = countMean(i, meanWindow);
	}

	return result;
}

QVector<int> LidarWorker::readRaw() const
{
	return mResult;
}

void LidarWorker::waitUntilInited()
{
	mWaitForInit.acquire(1);
	mWaitForInit.release(1);
}

void LidarWorker::onNewData(const QVector<uint8_t> &data)
{
	Q_UNUSED(data)
	mBufferLock.lock();
	mBuffer.append(mFifo->readRaw());
	processBuffer();
	mBufferLock.unlock();
}

int LidarWorker::countMean(const int i, const int meanWindow) const
{
	auto max = 0;
	auto min = 1000 * 1000 * 1000; // big int
	auto mean = 0;
	auto meanCounter = 0;
	for (auto j = i; j  < (i + meanWindow) % ANGLES_RAW_NUMBER; ++j) {
		j %= ANGLES_RAW_NUMBER;
		if (mResult[j] != 0) {
			max = std::max(max, mResult[j]);
			min = std::min(min, mResult[j]);
			mean += mResult[j];
			meanCounter++;
		}
	}
	if (meanCounter > 2) {
		return (mean - min - max) / (meanCounter - 2); // [150, 250) = 2
	} else {
		if (meanCounter == 0) {
			return mean;
		} else {
			return mean / meanCounter;
		}
	}
}

void LidarWorker::processBuffer()
{
	while (!mBuffer.isEmpty()) {
		int startByte = 0;
		while (mBuffer[startByte] != PKG_HEADER) {
			startByte++;
			if (startByte == mBuffer.size()) {
				mBuffer.clear();
				return;
			}
		}
		if (mBuffer.size() - 1 < startByte + DATA_HEADER_BYTE + 1) {
			mBuffer = mBuffer.mid(startByte);
			return;
		}
		if (!checkProtocol(mBuffer, startByte)) {
			mBuffer = mBuffer.mid(startByte + 1);
			return;
		}

		uint16_t rawDataLength = (mBuffer[startByte + SIZE_BYTE] << 8) | mBuffer[startByte + SIZE_BYTE + 1];
		if (mBuffer.size() - 1 < startByte + rawDataLength + 1) {
			mBuffer = mBuffer.mid(startByte);
			return;
		}

		if (checkChecksum(mBuffer, startByte, rawDataLength)) {
			processData(mBuffer.mid(startByte, rawDataLength));
			mBuffer = mBuffer.mid(startByte + rawDataLength);
		}
		else {
			mBuffer = mBuffer.mid(startByte + 1);
		}
	}
}

void LidarWorker::processData(const QVector<uint8_t> &data)
{
	uint16_t dataLength = (data[DATA_SIZE_BYTE] << 8) | data[DATA_SIZE_BYTE + 1];
	uint16_t startAngle = (data[START_ANGLE_BYTE] << 8) | data[START_ANGLE_BYTE + 1];
	int readNumber = (dataLength - 5) / 3;
	for (auto i = 0; i < readNumber; ++i) {
		auto angle = startAngle + ANGLE_STEP * i / readNumber;
		auto distByte = FIRST_DIST_BYTE + i * 3;
		auto distance = ((data[distByte] << 8) + data[distByte + 1]) * 0.25;
		mResult[angle] = distance;
	}
}

bool LidarWorker::checkProtocol(const QVector<uint8_t> &data, uint start)
{
	return data[start + PROTOCOL_BYTE] == PROTOCOL_VERSION &&
			data[start + TYPE_BYTE] == PKG_TYPE &&
			data[start + DATA_HEADER_BYTE] == DATA_HEADER;
}

bool LidarWorker::checkChecksum(const QVector<uint8_t> &data, uint start, uint size)
{
	uint16_t checksum = (data[start + size] << 8) | data[start + size + 1];
	return checksum == (std::accumulate(data.begin() + start, data.begin() + start + size, 0) & 0xffff);
}

