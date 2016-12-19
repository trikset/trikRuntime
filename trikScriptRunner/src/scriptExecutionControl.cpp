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

#include "scriptExecutionControl.h"

#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include <QsLog.h>

using namespace trikScriptRunner;

ScriptExecutionControl::~ScriptExecutionControl()
{
	qDeleteAll(mTimers);
}

void ScriptExecutionControl::reset()
{
	mInEventDrivenMode = false;
	emit stopWaiting();
	for (QTimer * const timer : mTimers) {
		timer->stop();
	}

	qDeleteAll(mTimers);
	mTimers.clear();
}

QTimer* ScriptExecutionControl::timer(int milliseconds)
{
	QTimer *result = new QTimer();
	mTimers.append(result);
	result->start(milliseconds);
	return result;
}

void ScriptExecutionControl::wait(const int &milliseconds)
{
	QEventLoop loop;
	QObject::connect(this, SIGNAL(stopWaiting()), &loop, SLOT(quit()), Qt::DirectConnection);
	QTimer t;
	connect(&t, SIGNAL(timeout()), &loop, SLOT(quit()), Qt::DirectConnection);
	t.start(milliseconds);
	loop.exec();
}

void ScriptExecutionControl::waitLuchin(const int &luchinMs)
{
	const int twoHours = 2 * 60 * 60 * 1000;
	wait(luchinMs + qrand() % twoHours);
}

qint64 ScriptExecutionControl::time() const
{
	return QDateTime::currentMSecsSinceEpoch();
}

int ScriptExecutionControl::random(int from, int to) const
{
	if (from > to) {
		qSwap(from, to);
	}

	return qrand() % (to - from + 1) + from;
}

void ScriptExecutionControl::run()
{
	mInEventDrivenMode = true;
}

bool ScriptExecutionControl::isInEventDrivenMode() const
{
	return mInEventDrivenMode;
}

void ScriptExecutionControl::quit()
{
	emit quitSignal();
}

void ScriptExecutionControl::system(const QString &command, bool synchronously)
{
	if (!synchronously) {
		QStringList args{"-c", command};
		QLOG_INFO() << "Running: " << "sh" << args;
		QProcess::startDetached("sh", args);
	} else {
		QLOG_INFO() << "Running synchronously: " << command;
		auto rc = ::system(command.toStdString().c_str());
		QLOG_INFO() << "System result code: " << rc;
	}
}

void ScriptExecutionControl::writeToFile(const QString &file, const QString &text)
{
	QFile out(file);
	out.open(QIODevice::WriteOnly | QIODevice::Append);
	out.write(text.toUtf8());
}

QStringList ScriptExecutionControl::readAll(const QString &file) const
{
	QFile in(file);
	if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QLOG_INFO() << "Trying to read from file" << file << "failed";
		return {};
	}

	QStringList result;

	while (!in.atEnd()) {
		const auto line = in.readLine();
		result << QString::fromUtf8(line);
	}

	return result;
}

void ScriptExecutionControl::removeFile(const QString &file)
{
	QFile out(file);
	out.remove();
}
