/* Copyright 2015 CyberTech Labs Ltd.
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

#include "declSpec.h"

class QTimer;

namespace trikControl {

/// Interface representing TRIK system and script execution controlling functions.
class TRIKCONTROL_EXPORT ScriptInterface : public QObject
{
	Q_OBJECT

public:
	/// Returns true if a script is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	virtual bool isInEventDrivenMode() const = 0;

	/// Resets script execution state, clearing all flags and stopping all timers.
	virtual void reset() = 0;

public slots:
	/// Starts a new timer with given interval and returns reference to it.
	virtual QTimer *timer(int milliseconds) = 0;

	/// Waits given amount of time in milliseconds and returns.
	virtual void wait(int const &milliseconds) = 0;

	/// Returns the number of milliseconds since 1970-01-01T00:00:00 UTC.
	virtual qint64 time() const = 0;

	/// Starts event loop for script.
	virtual void run() = 0;

	/// Aborts script execution.
	virtual void quit() = 0;

	/// Asynchronously execute given sh command.
	virtual void system(QString const &command) = 0;

signals:
	/// Emitted when script requested system to abort execution.
	void quitSignal();

	/// To be connected to quit() slot of local event loops that are used for waiting.
	void stopWaiting();
};

}
