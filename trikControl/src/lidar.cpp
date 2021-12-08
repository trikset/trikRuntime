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

using namespace trikControl;

const uint8_t PKG_HEADER = 0xAA;
const int SIZE_BYTE = 1;

const uint8_t PROTOCOL_VERSION = 0x01;
const int PROTOCOL_BYTE = 3;

const uint8_t PKG_TYPE = 0x01;
const int TYPE_BYTE = 4;

const uint8_t DATA_HEADER = 0xAD;
const int DATA_START_BYTE = 5;

const int DATA_SIZE_BYTE = 6;
const int START_ANGLE_BYTE = 11;
const int FIRST_DIST_BYTE = 14;

const int ANGLES_NUMBER = 3600;
const int ANGLE_STEP = ANGLES_NUMBER / 16;

Lidar::Lidar(const QString &port, const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Lidar on" + port)
	, mFifo(new Fifo(port, configurer, hardwareAbstraction))
	, mResult(ANGLES_NUMBER, 0)
	, mIsResultChanged(ANGLES_NUMBER, true)
{
}

Lidar::~Lidar()
{
}

Lidar::Status Lidar::status() const
{
	return Status::off;
}

QVector<int> Lidar::read() const
{
	QVector<int> result(360, 0);

	auto it = std::find(mIsResultChanged.begin(), mIsResultChanged.end(), true);
	while (it != mIsResultChanged.end()) {
		int index = it - mIsResultChanged.begin();
		int max = mIsResultChanged[index];
		int min = mIsResultChanged[index];
		int mean = 0;
		const int indexWas = index;
		while(index % 5 != 0 || index % 10 == 0) { // count (15, 25]
			mIsResultChanged[index] = false;
			max = std::max(max, mResult[index]);
			min = std::min(min, mResult[index]);
			mean += mResult[index];
			index--;
		}
		index = indexWas + 1;
		do {
			mIsResultChanged[index] = false;
			max = std::max(max, mResult[index]);
			min = std::min(min, mResult[index]);
			mean += mResult[index];
			index++;
		} while(index % 5 != 0 || index % 10 == 0);

		result[(indexWas + 4) / 10] = (mean - min - max) / 8; // (15, 25] = 2
		it = std::find(it, mIsResultChanged.end(), true);
	}

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
	for (int i = 0; i < readNumber; ++i) {
		int angle = startAngle + ANGLE_STEP * (i - 1) / readNumber;
		int distByte = FIRST_DIST_BYTE + i * 3;
		int distance = ((data[distByte] << 8) + data[distByte + 1]) * 0.25;
		mResult[angle] = distance;
		mIsResultChanged[angle] = true;
	}
}

bool Lidar::checkProtocol(const QVector<uint8_t> &data, int start, int size)
{
	uint16_t checksum = (data[start + size] << 8) + data[start + size + 1];
	return data[start + PROTOCOL_BYTE] == PROTOCOL_VERSION &&
			data[start + TYPE_BYTE] == PKG_TYPE &&
			data[start + DATA_START_BYTE] == DATA_HEADER &&
			checksum == countChecksum(data, start, start + size);
}

uint16_t countChecksum(const QVector<uint8_t> &data, int start, int end) {
	uint16_t result = 0;
	for (int i = start; i < end; i++) {
		result += data[i];
	}
	return result;
}
