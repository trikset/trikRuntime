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

#include "lidar.h"

#include <trikKernel/configurer.h>
#include <QDebug>
#include <QsLog.h>

using namespace trikControl;

constexpr uint8_t PKG_HEADER = 0xAA;
constexpr int SIZE_BYTE = 1;

constexpr uint8_t PROTOCOL_VERSION = 0x01;
constexpr int PROTOCOL_BYTE = 3;

constexpr uint8_t PKG_TYPE = 0x61;
constexpr int TYPE_BYTE = 4;

constexpr uint8_t DATA_HEADER = 0xAD;
constexpr int DATA_START_BYTE = 5;

constexpr int DATA_SIZE_BYTE = 6;
constexpr int START_ANGLE_BYTE = 11;
constexpr int FIRST_DIST_BYTE = 14;

constexpr int ANGLES_RAW_NUMBER = 3600;
constexpr int ANGLE_STEP = ANGLES_RAW_NUMBER / 16;
constexpr int ANGLES_NUMBER = 360;

Lidar::Lidar(const QString &port, const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Lidar on" + port)
	, mFifo(new Fifo(port, configurer, hardwareAbstraction))
	, mResult(ANGLES_RAW_NUMBER, 0)
{
	connect(mFifo, &Fifo::newData, this, &Lidar::onNewData);
	qDebug() << "LIdAR";
}

Lidar::Status Lidar::status() const
{
	return mFifo->status();
}

QVector<int> Lidar::read() const
{
	QVector<int> result(ANGLES_NUMBER, 0);
	constexpr int meanWindow = ANGLES_RAW_NUMBER / ANGLES_NUMBER; // 10
	constexpr int halfWindow = meanWindow / 2; // 5
	for (auto i = halfWindow; i < mResult.size() - halfWindow; i += meanWindow) {
		auto max = mResult[i];
		auto min = mResult[i];
		int mean = 0;
		for (auto j = i; j < i + meanWindow; ++j) {
			max = std::max(max, mResult[j]);
			min = std::min(min, mResult[j]);
			mean += mResult[j];
		}
		result[(i + halfWindow) / meanWindow] = (mean - min - max) / (meanWindow - 2); // [15, 25) = 2
	}

	auto max = mResult[0];
	auto min = mResult[0];
	for (auto i = 0; i < halfWindow; ++i) { // from 3595 to 5
		max = std::max(max, mResult[i]);
		min = std::min(min, mResult[i]);
		result[0] += mResult[i];
	}
	for (auto i = mResult.size() - halfWindow; i < mResult.size(); ++i) {
		max = std::max(max, mResult[i]);
		min = std::min(min, mResult[i]);
		result[0] += mResult[i];
	}
	result[0] = (result[0] - min - max) / (meanWindow - 2);

	return result;
}

QVector<int> Lidar::readRaw() const
{
	return mResult;
}

void Lidar::onNewData(const QVector<uint8_t> &data)
{
	mBuffer.append(data);
	processBuffer();
}

void Lidar::processBuffer()
{
	QLOG_INFO() << "LIDAR" << __PRETTY_FUNCTION__ << "\n" << mBuffer;
	while (!mBuffer.isEmpty()) {
		int startByte = 0;
		while (mBuffer[startByte] != PKG_HEADER) {
			startByte++;
			if (startByte == mBuffer.size()) {
				mBuffer.clear();
				return;
			}
		}
		if (mBuffer.size() - 1 < startByte + SIZE_BYTE + 1) {
			mBuffer = mBuffer.mid(startByte);
			return;
		}
		uint16_t rawDataLength = (mBuffer[startByte + SIZE_BYTE] << 8) + mBuffer[startByte + SIZE_BYTE + 1];
		if (mBuffer.size() - 1 < startByte + rawDataLength + 1) {
			mBuffer = mBuffer.mid(startByte);
			return;
		}

		if (checkProtocol(mBuffer, startByte, rawDataLength)) {
			processData(mBuffer.mid(startByte, rawDataLength));
			mBuffer = mBuffer.mid(startByte + rawDataLength);
		}
		else {
			mBuffer = mBuffer.mid(startByte + 1);
		}
	}
}

void Lidar::processData(const QVector<uint8_t> &data)
{
	uint16_t dataLength = (data[DATA_SIZE_BYTE] << 8) + data[DATA_SIZE_BYTE + 1];
	uint16_t startAngle = (data[START_ANGLE_BYTE] << 8) + data[START_ANGLE_BYTE + 1];
	int readNumber = (dataLength - 5) / 3;
	for (auto i = 0; i < readNumber; ++i) {
		auto angle = startAngle + ANGLE_STEP * (i - 1) / readNumber;
		auto distByte = FIRST_DIST_BYTE + i * 3;
		auto distance = ((data[distByte] << 8) + data[distByte + 1]) * 0.25;
		mResult[angle] = distance;
	}
}

bool Lidar::checkProtocol(const QVector<uint8_t> &data, uint start, uint size)
{
	uint16_t checksum = (data[start + size] << 8) + data[start + size + 1];
	return data[start + PROTOCOL_BYTE] == PROTOCOL_VERSION &&
			data[start + TYPE_BYTE] == PKG_TYPE &&
			data[start + DATA_START_BYTE] == DATA_HEADER &&
			checksum == (std::accumulate(data.begin(), data.end(), 0) & 0xffff);
}
