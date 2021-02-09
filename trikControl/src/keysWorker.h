/* Copyright 2014 - 2016 CyberTech Labs Ltd.
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
#include <QtCore/QScopedPointer>
#include <QtCore/QSet>
#include <QtCore/QReadWriteLock>

#include <trikHal/hardwareAbstractionInterface.h>

#include "deviceState.h"

namespace trikControl {

/// Watches for keys on a brick, intended to work in separate thread.
class KeysWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param keysPath - path to device file that controls brick keys.
	/// @param state - device state, worker sets it to "failed" if it can not open file.
	/// @param hardwareAbstraction - interface to TRIK hardware.
	KeysWorker(const QString &keysPath, DeviceState &state
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	/// Execute init() after worker thread started.
	void init();

	/// Clear data about previous key pressures.
	void reset();

public slots:
	/// Returns true if a key with given code was pressed.
	bool wasPressed(int code);

private slots:
	void readKeysEvent(int eventType, int code, int value, const trikKernel::TimeVal &eventTime);

signals:
	/// Triggered when button state changed (pressed or released).
	/// @param code - key code.
	/// @param value - key state.
	void buttonPressed(int code, int value);

	/// Triggers when everyone who waits for buttons events has to stop waiting.
	void stopWaiting();

private:
	QScopedPointer<trikHal::EventFileInterface> mEventFile;
	int mButtonCode = 0;
	int mButtonValue = 0;
	QSet<int> mWasPressed;
	QReadWriteLock mLock;

	const trikHal::HardwareAbstractionInterface &mHardwareAbstraction;
	const QString mKeysPath;

	/// Device state object, shared between worker and proxy.
	DeviceState &mState;
};

}
