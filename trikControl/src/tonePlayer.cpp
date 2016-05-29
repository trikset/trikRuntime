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


#include "tonePlayer.h"

#include <QsLog.h>

namespace trikControl{

TonePlayer::TonePlayer()
	:	mdevice(0)
	,	moutput(0)
{

	mtimer.setSingleShot(true);
	initializeAudio();
	mdevice = new AudioSynthDeviceBuffered(this, m_format.sampleRate(), m_format.sampleSize());
	moutput = new QAudioOutput(m_format, this);
}

void TonePlayer::initializeAudio()
{

	m_format.setChannelCount(1);
	m_format.setSampleRate(44100);
	m_format.setSampleSize(8);
	m_format.setSampleType(QAudioFormat::SampleType::SignedInt);
	m_format.setCodec("audio/pcm");

	connect(&mtimer, SIGNAL(timeout()), this, SLOT(stop()));

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(m_format)) {
		m_format = info.nearestFormat(m_format);
	}
}

void TonePlayer::play(int hzFreq, int msDuration)
{

	moutput->reset();
	switch (moutput->state()) {
		case QAudio::IdleState: break;
		default:break;
	}

	mtimer.setInterval(msDuration);
	mdevice->start(hzFreq);
	mtimer.start();
	moutput->start(mdevice);
}

void TonePlayer::stop()
{
	mtimer.stop();
	moutput->stop();

}
}


