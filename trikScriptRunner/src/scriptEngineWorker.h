/* Copyright 2013 Yurii Litvinov
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

#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

#include <trikControl/brick.h>

namespace trikScriptRunner
{

/// Worker object to be run in a separate thread for Qt Script execution. QScriptEngine calls ProcessEvents too
/// infrequently even when ProcessEventsInterval is set to 1 ms, so there is a need for separate threads to
/// run a script and listen for incoming connections.
class ScriptEngineWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configFilePath - path to a config file for trikControl.
	ScriptEngineWorker(QString const &configFilePath);

	/// Returns true, if script engine is initialized and running.
	bool isRunning() const;

	/// Aborts script execution.
	void abort();

	/// Returns true if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool isInEventDrivenMode() const;

signals:
	/// Fired when current script execution completed.
	void completed();

public slots:
	/// Executes given script.
	void run(QString const &script);

private slots:
	/// Abort script execution.
	void onScriptRequestingToQuit();

	/// Do cleanup when script engine finally is deleted.
	void onScriptEngineDestroyed();

private:
	void initScriptEngine();

	// Has ownership. No smart pointers here because we need to do manual memory managment
	// due to complicated mEngine lifecycle (see .cpp for more details).
	QScriptEngine *mEngine;

	trikControl::Brick mBrick;
};

}
