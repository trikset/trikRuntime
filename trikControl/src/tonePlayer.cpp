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


namespace trikControl{

TonePlayer::TonePlayer()
{
	mTimer.setSingleShot(true);
	initializeAudio();
	mDevice = new AudioSynthDevice(this, mFormat.sampleRate(), mFormat.sampleSize());
	mOutput = new QAudioOutput(mFormat, this);
}

void TonePlayer::initializeAudio()
{

	mFormat.setChannelCount(1);
	mFormat.setSampleRate(16000);
	mFormat.setSampleSize(16);
	mFormat.setSampleType(QAudioFormat::SampleType::SignedInt);
	mFormat.setCodec("audio/pcm");

	connect(&mTimer, SIGNAL(timeout()), this, SLOT(stop()));

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(mFormat)) {
		mFormat = info.nearestFormat(mFormat);
	}
}

void TonePlayer::play(int hzFreq, int msDuration)
{
	mDevice->start(hzFreq);
	switch (mOutput->state()) {
		case QAudio::ActiveState:
			mOutput->suspend();
			mDevice->reset();
			mOutput->resume();
			break;
		case QAudio::SuspendedState: mOutput->resume(); break;
		case QAudio::StoppedState:   mOutput->start(mDevice); break;
		case QAudio::IdleState:      mOutput->start(mDevice); break;
		default: break;
	}

	mTimer.setInterval(msDuration);
	mTimer.start();
}

void TonePlayer::stop()
{
	mDevice->stop();
	mTimer.stop();
}
}
