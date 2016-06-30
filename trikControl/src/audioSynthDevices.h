/* Copyright 2016 Artem Sharganov, Iakov Kirilenko
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

#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QIODevice>
#include <QtMultimedia/QAudioFormat>

/// QIODevice that synthesize sine wave values
class AudioSynthDevice : public QIODevice
{
	Q_OBJECT

public:
	/// Constructor
	AudioSynthDevice(QObject *parent, int sampleRate, int sampleSize);

	~AudioSynthDevice();

	/// Provides reading from device
	qint64 readData(char *data, qint64 maxlen);

	/// Opens device, run generation in buffered mode
	void start(int hzFreq);

	/// Close device and reset pose
	void stop();

	/// Stub, because readonly device
	qint64 writeData(const char *data, qint64 len);

	/// Returns amount of available bytes
	qint64 bytesAvailable() const;

private:
	/// Sythesize sine wave values
	int generate(char *data, int lengthBytes);

private:

	/// Internal buffer, used in buffered mode
	QByteArray mBuffer;

	qint64 mPos;

	int mHzFreq;

	const int mSampleRate;

	const int mSampleSize;

	/// Mode of device
	const bool mBuffered = false;

	long long mY1 = 0;

	long long mY2 = 0;

	long long mB = 0;

	static const int M = 1 << 30;

	double mOmega;
};

