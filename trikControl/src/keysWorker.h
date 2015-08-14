/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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
#include <QtCore/QSocketNotifier>
#include <QtCore/QScopedPointer>
#include <QtCore/QSet>
#include <QtCore/QReadWriteLock>

#include "deviceState.h"

namespace trikControl {

/// Watches for keys on a brick, intended to work in separate thread.
class KeysWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param keysPath - path to device file that controls brick keys.
	KeysWorker(const QString &keysPath, DeviceState &state);

	/// Clear data about previous key pressures.
	void reset();

public slots:
	/// Returns true if a key with given code was pressed.
	bool wasPressed(int code);

private slots:
	void readKeysEvent();

signals:
	/// Triggered when button state changed (pressed or released).
	/// @param code - key code.
	/// @param value - key state.
	void buttonPressed(int code, int value);

private:
	QScopedPointer<QSocketNotifier> mSocketNotifier;
	int mKeysFileDescriptor;
	int mButtonCode;
	int mButtonValue;
	QSet<int> mWasPressed;
	QReadWriteLock mLock;

	/// Device state object, shared between worker and proxy,
	DeviceState &mState;
};

}
