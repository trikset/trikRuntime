/* Copyright 2016 Artem Sharganov and Iakov Kirilenko
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

#include "audioSynthDevice.h"

#include <QtMultimedia/QAudioDeviceInfo>
#include <qmath.h>
#include <QsLog.h>

AudioSynthDevice::AudioSynthDevice(int sampleRate, int sampleSize, QObject *parent)
	: QIODevice(parent)
	, mBuffer(0)
	, mPos(0)
	, mHzFreq(0)
	, mSampleRate(sampleRate)
	, mSampleSize(sampleSize)
{
}

void AudioSynthDevice::start(int hzFreq)
{
	if (!open(QIODevice::ReadOnly)) {
		QLOG_ERROR() << "Failed to open audio synthesis device";
		return;
	}
	if (!reset()) {
		QLOG_ERROR() << "Failed to reset audio synthesis device";
		return;
	}
	mPos = 0;
	mHzFreq = hzFreq;

	mOmega = mHzFreq * M_PI / mSampleRate;
	mY1 = -std::sin(mOmega) * M;
	mY2 = -std::sin(2 * mOmega) * M;
	mB = 2.0 * cos(mOmega) * M;

	if (mBuffered) {
		const qint64 length = (mSampleRate * (mSampleSize / 8));
		mBuffer.resize(length);
		generate(mBuffer.data(), length);
	}
	emit readyRead();
}

void AudioSynthDevice::stop()
{
	mHzFreq = 0;
}

// Modified coupled first-order form algorithm with fixed point arithmetic
int AudioSynthDevice::generate(char *data, int length)
{
	if(mHzFreq == 0) {
		return 0;
	}

	const int channelBytes = mSampleSize / 8;

	const qint64 maxlen = length / channelBytes;

	const int AMPLITUDE = (1 << (mSampleSize - 1)) - 1;

	auto ptr = static_cast<unsigned char *>(static_cast<void*>(data));

	long long y0;

	int i = 0;

	for (i = 0; i < maxlen; ++i) {
		y0 = mB * mY1 / M - mY2;
		mY2 = mB * y0 / M - mY1;
		mY1 = mB * mY2 / M - y0;

		if (mSampleSize == 8) {
			const qint8 val = static_cast<qint8>(y0 * AMPLITUDE / M);
			*static_cast<qint8*>(static_cast<void*>(ptr)) = val;
		} else if(mSampleSize == 16) {
			const qint16 val = static_cast<qint16>(y0 * AMPLITUDE / M);
			*static_cast<qint16*>(static_cast<void*>(ptr)) = val;
		}

		ptr += channelBytes;
	}

	return i * channelBytes;
}

qint64 AudioSynthDevice::readData(char *data, qint64 len)
{
	if (mBuffered) {
		qint64 total = 0;
		while (len - total > 0) {
			const qint64 chunk = qMin((mBuffer.size() - mPos), len - total);
			memcpy(data + total, mBuffer.constData() + mPos, chunk);
			mPos = (mPos + chunk) % mBuffer.size();
			total += chunk;
		}

		return total;
	} else {
		return generate(data, len);
	}
}

qint64 AudioSynthDevice::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data)
	Q_UNUSED(len)

	return 0;
}

qint64 AudioSynthDevice::bytesAvailable() const
{
	return mBuffer.size() + QIODevice::bytesAvailable();
}
