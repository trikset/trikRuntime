/* Copyright 2016 Artem Sharganov
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

#include "tonePlayer.h"

#include <QsLog.h>

using namespace trikControl;

TonePlayer::TonePlayer()
{
	mTimer.setSingleShot(true);
	initializeAudio();
	mDevice = new AudioSynthDevice(mFormat.sampleRate(), mFormat.sampleSize(), this);
	mOutput = new QAudioOutput(mFormat, this);
}

void TonePlayer::initializeAudio()
{
	mFormat.setChannelCount(CHANNEL_COUNT);
	mFormat.setSampleRate(SAMPLE_RATE);
	mFormat.setSampleSize(SAMPLE_SIZE);
	mFormat.setSampleType(QAudioFormat::SampleType::SignedInt);
	mFormat.setCodec("audio/pcm");

	connect(&mTimer, &QTimer::timeout, this, &TonePlayer::stop);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(mFormat)) {
		mFormat = info.nearestFormat(mFormat);
		QLOG_INFO() << "Specified format is not supported. The nearest one is:"
					<< "channel count: " << mFormat.channelCount() << ";"
					<< "sample rate: " << mFormat.sampleRate() << ";"
					<< "sample size: " << mFormat.sampleSize() << ";"
					<< "sample type: " << mFormat.sampleType() << ";"
					<< "codec: " << mFormat.codec();
	}
}

void TonePlayer::play(int freqHz, int durationMs)
{
	mDevice->start(freqHz);
	const auto state = mOutput->state();
	QLOG_INFO() << "Device started. Output state is" << state;
	switch (state) {
		case QAudio::ActiveState:
			mOutput->suspend();
			mDevice->reset();
			mOutput->resume();
			break;
		case QAudio::SuspendedState:
			mOutput->resume();
			break;
		case QAudio::StoppedState:
			mOutput->start(mDevice);
			break;
		case QAudio::IdleState:
			mOutput->start(mDevice);
			break;
		case QAudio::InterruptedState:
				QLOG_ERROR() << "Audio device was interrupted previously";
				mOutput->start(mDevice);
			break;
	}

	mTimer.setInterval(durationMs);
	mTimer.start();
}

void TonePlayer::stop()
{
	mTimer.stop();
	mOutput->suspend();
	mOutput->reset();
}
