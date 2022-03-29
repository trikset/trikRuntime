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

#include <QsLog.h>
#include <QtAlgorithms>
#include <stdint.h>

struct Delta2AProbe {
	uint8_t signal;
	uint16_t dist;
} Q_PACKED;

struct Delta2ALayout {
	uint8_t  pkg_hdr_magic;
	uint16_t pkg_hdr_length;
	uint8_t  version;
	uint8_t  type;
	uint8_t  data_hdr_magic;
	uint16_t data_hdr_length;
	uint8_t  speed;
	uint16_t zero_offset;
	uint16_t start_angle;
} Q_PACKED;

using namespace trikControl;

constexpr uint8_t PKG_HEADER_MAGIC = 0xAA;
constexpr uint8_t PROTOCOL_VERSION = 0x01;
constexpr uint8_t PKG_TYPE = 0x61;
constexpr uint8_t DATA_HEADER_MAGIC = 0xAD;

constexpr int ANGLES_RAW_NUMBER = 36000;
constexpr int ANGLE_STEP = ANGLES_RAW_NUMBER / 16;
constexpr int ANGLES_NUMBER = 360;

constexpr qint64 LIDAR_DATA_CHUNK_SIZE = 4096;

static uint16_t get_unaligned_be16(const void *p) {
	uint16_t ret = (uint16_t)*(const uint8_t*)p;
	ret = (ret << 8) + *(const uint8_t*)(((const uint8_t*)p)+1);
	return ret;
}

trikControl::LidarWorker::LidarWorker(const QString &fileName
					, const trikHal::HardwareAbstractionInterface &)
	: mSerial(fileName)
	, mLidarChunk(new char[LIDAR_DATA_CHUNK_SIZE])
	, mResult(ANGLES_RAW_NUMBER, 0)
	, mStatus(Status::off)
{
	mWaitForInit.acquire(1);
}

LidarWorker::~LidarWorker()
{
	delete[] mLidarChunk;
}

LidarWorker::Status LidarWorker::status() const
{
	return mStatus;
}

void LidarWorker::init()
{
	mStatus = Status::starting;
	if (!mSerial.open(QIODevice::ReadOnly)) {
		QLOG_ERROR() << "Lidar: failed to open serial port " << mSerial.portName() << " in read-only mode: " << mSerial.error();
		mStatus = Status::permanentFailure;
		mWaitForInit.release(1);
		return;
	}
	mSerial.setBaudRate(230400);
	mSerial.setDataBits(QSerialPort::Data8);
	mSerial.setParity(QSerialPort::NoParity);
	mSerial.setStopBits(QSerialPort::OneStop);
	mSerial.setFlowControl(QSerialPort::NoFlowControl);

	connect(&mSerial, &QSerialPort::readyRead, this, &LidarWorker::readData);
	mStatus = Status::ready;

	QLOG_INFO() << "Lidar: opened serial port" << mSerial.portName();
	mWaitForInit.release(1);
}

QVector<int> LidarWorker::read() const
{
	QVector<int> result(ANGLES_NUMBER, 0);
	constexpr int meanWindow = ANGLES_RAW_NUMBER / ANGLES_NUMBER; // 100
	constexpr int halfWindow = meanWindow / 2; // 50
	for (auto i = halfWindow; i < mResult.size(); i += meanWindow) {
		result[i / meanWindow] = countMean(i, meanWindow);
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

void LidarWorker::readData()
{
	uint8_t bytes[256];
	struct Delta2ALayout *s = (struct Delta2ALayout *)mLidarChunk;

	while (!mSerial.atEnd()) {
		// read data block from serial port
		auto bytesRead = mSerial.read((char *)bytes, sizeof(bytes));
		if (bytesRead == 0)
			return;
		if (bytesRead < 0) {
			QLOG_ERROR() << "Lidar: read failed: " << mSerial.error() << " in " << mSerial.portName();
			return;
		}

		// feed the data to very simple FSM
		for (int i = 0; i < bytesRead; i++) {
			if (mFlagHunt) {
				if (bytes[i] == PKG_HEADER_MAGIC) {
					mLidarChunk[0] = bytes[i];
					mLidarChunkBytes = 1;
					mFlagHunt = false;
				}
			} else {
				mLidarChunk[mLidarChunkBytes++] = bytes[i];
				if (mLidarChunkBytes == sizeof(struct Delta2ALayout)) {
					// now we have a header
					// perform basic sanity checks
					if (s->pkg_hdr_magic != PKG_HEADER_MAGIC) {
						QLOG_ERROR() << "Lidar: invalid pkg header magic";
						mFlagHunt = true;
						continue;
					}
					if (s->version != PROTOCOL_VERSION) {
						QLOG_ERROR() << "Lidar: invalid protocol version";
						mFlagHunt = true;
						continue;
					}
					if (s->type != PKG_TYPE) {
						QLOG_ERROR() << "Lidar: invalid pkg type";
						mFlagHunt = true;
						continue;
					}
					if (s->data_hdr_magic != DATA_HEADER_MAGIC) {
						QLOG_ERROR() << "Lidar: invalid data header magic";
						mFlagHunt = true;
						continue;
					}
					if (get_unaligned_be16(&(s->pkg_hdr_length))+2 > LIDAR_DATA_CHUNK_SIZE) {
						QLOG_ERROR() << "Lidar: chunk length too long";
						mFlagHunt = true;
						continue;
					}
					if (get_unaligned_be16(&(s->pkg_hdr_length)) != get_unaligned_be16(&(s->data_hdr_length))+8) {
						QLOG_ERROR() << "Lidar: chunk length and data length mismatch";
						mFlagHunt = true;
						continue;
					}
				}
				if (mLidarChunkBytes > sizeof(struct Delta2ALayout) &&
				    mLidarChunkBytes == (size_t)(get_unaligned_be16(&(s->pkg_hdr_length)) + 2)) {
					// We've done reading the chunk
					if (checkChecksum(mLidarChunk, get_unaligned_be16(&(s->pkg_hdr_length)))) {
						processData(s);
					} else {
						QLOG_ERROR() << "Lidar: chunk checksum mismatch";
					}
					mFlagHunt = true;
				}
			}
		}
	}
}

int LidarWorker::countMean(const int i, const int meanWindow) const
{
	auto max = 0;
	auto min = 1000 * 1000 * 1000; // big int
	auto mean = 0U;
	auto meanCounter = 0U;
	for (auto j = i; j  < (i + meanWindow) % ANGLES_RAW_NUMBER; ++j) {
		j %= ANGLES_RAW_NUMBER;
		if (mResult[j] != 0) {
			max = std::max(max, mResult[j]);
			min = std::min(min, mResult[j]);
			mean += mResult[j];
			++meanCounter;
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

void LidarWorker::processData(const void *p)
{
	const struct Delta2ALayout *data = (const struct Delta2ALayout*)p;
	uint16_t dataLength = get_unaligned_be16(&(data->data_hdr_length));
	uint16_t startAngle = get_unaligned_be16(&(data->start_angle));
	int entries = (dataLength - 5) / 3;
	// Ugly, but c++-way, since gcc -pedantic forbids flexible array member
	const struct Delta2AProbe *probes = (const struct Delta2AProbe*)(((const char*)p)+sizeof(struct Delta2ALayout));

	// Inefficient. Consider switching to C-style arrays and drop QVector
	for (auto i = startAngle; i < startAngle + ANGLE_STEP; i++) {
		mResult[i] = 0;
	}
	for (auto i = 0; i < entries; ++i) {
		auto angle = startAngle + ANGLE_STEP * i / entries;
		mResult[angle] = get_unaligned_be16(&(probes[i].dist)) / 4;
	}
}

bool LidarWorker::checkChecksum(const char *data, size_t size)
{
	uint16_t result = 0;
	for (size_t i = 0; i < size; i++) {
		result += (uint8_t)(data[i]);
	}
	return result == get_unaligned_be16(data + size);
}
