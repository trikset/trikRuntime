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
#ifdef Q_OS_UNIX
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

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
	, mFileDescriptor(-1)
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
#ifndef Q_OS_UNIX
	mFifo.reset(new Fifo(mFifoFileName, mHardwareAbstraction));
	connect(mFifo.data(), &Fifo::newData, this, &LidarWorker::onNewData);
#else
	mFileDescriptor = ::open(mFifoFileName.toStdString().c_str(), O_NOCTTY | O_RDONLY | O_NONBLOCK | O_CLOEXEC);
	if (mFileDescriptor == -1) {
		QLOG_ERROR() << "Failed to open FIFO file in read-only mode" << mFifoFileName << " " << strerror(errno);
		mWaitForInit.release(1);
		return;
	}

	termios t {};
	QLOG_INFO() << "Using tty as FIFO:" << mFifoFileName;
	if (tcgetattr(mFileDescriptor, &t)) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ << ": tcgetattr failed for" << mFifoFileName;
	} else {
		t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
		t.c_oflag &= ~(OPOST);
		t.c_cflag |= CS8;
		t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
		t.c_cc[VMIN] = 1;
		t.c_cc[VTIME] = 0;
		termios t1 = t;
		cfsetospeed (&t, B230400);
		cfsetispeed (&t, B230400);
		tcsetattr(mFileDescriptor, TCSANOW, &t);
		if (tcgetattr(mFileDescriptor, &t1)
				|| std::make_tuple(t.c_iflag, t.c_oflag, t.c_cflag, t.c_lflag)
				!= std::make_tuple(t1.c_iflag, t1.c_oflag, t1.c_cflag, t1.c_lflag)
				|| cfgetispeed(&t1) != B230400
				|| cfgetospeed(&t1) != B230400) {
			QLOG_ERROR() << __PRETTY_FUNCTION__ << ": tcsetattr failed for" << mFifoFileName;
		}
	}

	mSocketNotifier.reset(new QSocketNotifier(mFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &LidarWorker::readFile);
	mSocketNotifier->setEnabled(true);

	QLOG_INFO() << "Opened FIFO file" << mFifoFileName;
#endif
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

void LidarWorker::readFile()
{
#ifdef Q_OS_UNIX
	QVector<uint8_t> bytes(4000);
	mSocketNotifier->setEnabled(false);
	auto bytesRead = ::read(mFileDescriptor, bytes.data(), static_cast<size_t>(bytes.size()));
	if (bytesRead < 0) {
		if (errno != EAGAIN) {
			QLOG_ERROR() << "FIFO read failed: " << strerror(errno) << "in" << mFifoFileName;
		}
		mSocketNotifier->setEnabled(true);
		return;
	}
	bytes.resize(bytesRead);
	mBuffer.append(bytes);
	processBuffer();
	mSocketNotifier->setEnabled(true);
#endif
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

	// Inefficient. Consider switching to C-style arrays and drop QVector
	for (auto i = startAngle; i < startAngle + ANGLE_STEP; i++) {
		mResult[i] = 0;
	}
	for (auto i = 0; i < readNumber; ++i) {
		auto angle = startAngle + ANGLE_STEP * i / readNumber;
		auto distByte = FIRST_DIST_BYTE + i * 3;
		auto distance = ((((int)(data[distByte])) << 8) + ((int)(data[distByte + 1]))) * 0.25;
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

