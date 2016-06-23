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

#include "audioSynthDevices.h"

#include <QtMultimedia/QAudioDeviceInfo>
#include <qmath.h>

AudioSynthDevice::AudioSynthDevice(QObject *parent, int sampleRate, int sampleSize)
	: QIODevice(parent)
	, mBuffer(0)
	, mPos(0)
	, mSampleRate(sampleRate)
	, mSampleSize(sampleSize)
{
}

AudioSynthDevice::~AudioSynthDevice()
{
}

void AudioSynthDevice::start(int hzFreq)
{
	open(QIODevice::ReadOnly);
	if (mBuffered) {
		const qint64 length = (mSampleRate * (mSampleSize / 8));
		mBuffer.resize(length);
		generate(mBuffer.data(), length, hzFreq);
	} else {
		mHzFreq = hzFreq;
	}
}

void AudioSynthDevice::stop()
{
	newCall = true;
	mPos = 0;
	close();
}

// Modefied coupled first-order form algorithm with fixed point arithmetic
int AudioSynthDevice::generate(char *data, int length, int hzFreq)
{
	const int channelBytes = mSampleSize / 8;

	qint64 maxlen = length / channelBytes;

	static const int M = 1 << 30;
	const auto w = hzFreq * M_PI / mSampleRate;
	const long long b1 = 2.0 * cos(w) * M;
	static const int AMPLITUDE = (1 << (mSampleSize - 1)) - 1;

	unsigned char *ptr = reinterpret_cast<unsigned char *>(data);

	// Need to save values between readData(...) calls, so static
	static long long y0 = 0;
	static decltype(y0) y1 = 0;
	static decltype(y0) y2 = 0;

	if (newCall) {
		y1 = M * std::sin(-w);
		y2 = M * std::sin(-2 * w);
		newCall = false;
	}

	int i = 0;

	for (i = 0; i < maxlen; ++i) {
		y0 = b1 * y1 / M - y2;
		y2 = b1 * y0 / M - y1;
		y1 = b1 * y2 / M - y0;

		if (mSampleSize == 8) {
			const qint8 val = static_cast<qint8>(y0 * AMPLITUDE / M);
			*reinterpret_cast<quint8*>(ptr) = val;
		} else if(mSampleSize == 16) {
			const qint16 val = static_cast<qint16>(y0 * AMPLITUDE / M);
			*reinterpret_cast<quint16*>(ptr) = val;
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
		return generate(data, len, mHzFreq);
	}
}

qint64 AudioSynthDevice::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data);
	Q_UNUSED(len);

	return 0;
}

qint64 AudioSynthDevice::bytesAvailable() const
{
	return mBuffer.size() + QIODevice::bytesAvailable();
}
