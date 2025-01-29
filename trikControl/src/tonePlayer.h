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

#pragma once

#include "audioSynthDevice.h"

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtMultimedia/QAudioOutput>

namespace trikControl {

/// Tone player. Play tones
class TonePlayer : public QObject
{
	Q_OBJECT
public:

	/// Constructor
	TonePlayer();

public Q_SLOTS:

	/// Play sound
	void play(int freqHz, int durationMs);

private:
	QAudioFormat mFormat;

	AudioSynthDevice *mDevice; // Has ownership
	QAudioOutput *mOutput; // Has ownership
	QTimer mTimer;

	void initializeAudio();

	static constexpr int CHANNEL_COUNT = 1;
	static constexpr int SAMPLE_RATE = 16000;
	static constexpr int SAMPLE_SIZE = 16;

public Q_SLOTS:
	/// Stop playing
	void stop();
};
}
