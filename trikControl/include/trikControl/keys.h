/* Copyright 2013 Matvey Bryksin, Yurii Litvinov
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

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QScopedPointer>

#include "declSpec.h"

namespace trikControl {

class KeysWorker;

/// Class for handling keys on a brick.
class TRIKCONTROL_EXPORT Keys : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param keysPath - path to device file that controls brick keys.
	Keys(QString const &keysPath);

	~Keys();

public slots:
	/// Returns true, if button with given code was pressed, and clears "pressed" state for that button.
	bool wasPressed(int code);

signals:
	/// Triggered when button state changed (pressed or released).
	/// @param code - key code.
	/// @param value - key state.
	void buttonPressed(int code, int value);

private:
	QScopedPointer<KeysWorker> mKeysWorker;
	QThread mWorkerThread;
};

}
