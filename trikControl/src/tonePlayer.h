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

#pragma once

#include "audioSynthDevices.h"

#include <QAudioOutput>
#include <QtCore/QObject>
#include <QTimer>


namespace trikControl {

/// Tone player. Play tones
class TonePlayer : public QObject
{
	Q_OBJECT

public:

	/// Constructor
	TonePlayer();

public slots:

	/// Play sound
	void play(int freqHz,int durationMs);

private:
	QAudioFormat m_format;

	AudioSynthDeviceBuffered *mdevice; // Has ownership.

	QAudioOutput *moutput; // Has ownership.

	QTimer mtimer;

	void initializeAudio();

public slots:
	void stop();
};
}
