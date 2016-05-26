/* Copyright 2013 - 2016 Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QAudioDeviceInfo>
#include <qmath.h>
#include <QsLog.h>



AudioSynthDeviceBuffered::AudioSynthDeviceBuffered(QObject *parent, int sampleRate, int sampleSize)
	: QIODevice(parent)
	, mBuffer(0)
	, mPos(0)
	, mSampleRate(sampleRate)
	, mSampleSize(sampleSize)
{

}

AudioSynthDeviceBuffered::~AudioSynthDeviceBuffered()
{

}

void AudioSynthDeviceBuffered::start(int hzFreq)
{
	open(QIODevice::ReadOnly);
	if(mBuffered){
		qint64 length = (mSampleRate * (mSampleSize / 8));
		mBuffer.resize(length*8);
		generate(mBuffer.data(), length, hzFreq);
	}
	else mHzFreq = hzFreq;
}

void AudioSynthDeviceBuffered::stop()
{
	mPos = 0;
	close();
}

int AudioSynthDeviceBuffered::generate(char *data, int length, int hzFreq)
{
	const int channelBytes = mSampleSize / 8;

	qint64 maxlen = length/mSampleSize;

	static const int M = 1 << 16;
	const float w = hzFreq * M_PI / mSampleRate;
	const long b1 = 2.0 * cos(w)*M;
	static const int AMPLITUDE = (1 << (mSampleSize - 1))>>1;

	unsigned char *ptr = reinterpret_cast<unsigned char *>(data);

	static int y1 = M * qSin(w);
	static int y2 = M * qSin(2*w);
	static int y0;

	int i = 0;

	for(i = 0; i < maxlen; ++i){

		y0 = b1*y1 / M - y2;
		y2 = b1*y0 / M - y1;
		y1 = b1*y2 / M - y0;

		if(mSampleSize == 8){
			const qint8 val = static_cast<qint8>(y0*AMPLITUDE/M);
			*reinterpret_cast<quint8*>(ptr) = val;
		}
		if(mSampleSize == 16)
		{
			const qint16 val = static_cast<qint16>(y0*AMPLITUDE/M);
			*reinterpret_cast<quint16*>(ptr) = val;
		}

		ptr+=channelBytes;
		maxlen-= channelBytes;
	}
	return i;
}

qint64 AudioSynthDeviceBuffered::readData(char *data, qint64 len)
{
	if(mBuffered)
	{
		qint64 total = 0;
		while (len - total > 0) {
			const qint64 chunk = qMin((mBuffer.size() - mPos), len - total);
			memcpy(data + total, mBuffer.constData() + mPos, chunk);
			mPos = (mPos + chunk) % mBuffer.size();
			total += chunk;
		}
		return total;
	}
	else return generate(data, len, mHzFreq);
}

qint64 AudioSynthDeviceBuffered::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data);
	Q_UNUSED(len);

	return 0;
}

qint64 AudioSynthDeviceBuffered::bytesAvailable() const
{
	return mBuffer.size() + QIODevice::bytesAvailable();
}
