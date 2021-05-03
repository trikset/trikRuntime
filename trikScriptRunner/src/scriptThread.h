/* Copyright 2014 CyberTech Labs Ltd.
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

#include <QtCore/QThread>
#include <QtScript/QScriptEngine>
#include "trikScriptRunnerDeclSpec.h"

namespace trikScriptRunner {

class Threading;

/// Thread object which executes a script with a QScriptEngine
class TRIKSCRIPTRUNNER_EXPORT ScriptThread : public QThread
{
	Q_OBJECT

public:
	/// Constructor
	/// @param threading - threading manager for this thread
	/// @param engine - QScriptEngine which will do the work
	/// @param script - a Qt Script to run
	ScriptThread(Threading &threading, const QString &id, QScriptEngine *engine, const QString &script);

	~ScriptThread() override;

	/// Aborts execution. Asynchronous: a thread will not be finished on return from this function
	void abort();

	/// @returns string id of the thread
	QString id() const;

	/// @returns error message or empty string if evalutation succeed
	QString error() const;

	/// @returns true if the script engine is evaluating a script at the moment
	bool isEvaluating() const;

signals:
	/// Emitted when event loop must be stopped.
	void stopRunning();

	/// Emitted when variables values are ready
	/// @param json - JSON container for variables values
	void variablesReady(const QJsonObject &json);

public slots:
	/// Catch variables values from current script execution
	/// @param propertyName - name of variables prefix, i.e prefix "web" for variable "web.light"
	void onGetVariables(const QString &propertyName);

protected:
	void run() override;

private:
	QString mId;

	QScopedPointer<QScriptEngine> mEngine;
	QString mScript;

	Threading &mThreading;

	QString mError;
};

}
