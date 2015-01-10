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

#include "script.h"

#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include <QsLog.h>

using namespace trikScriptRunner;

Script::~Script()
{
	qDeleteAll(mTimers);
}

void Script::reset()
{
	mInEventDrivenMode = false;
	emit stopWaiting();
	for (QTimer * const timer : mTimers) {
		timer->stop();
	}

	qDeleteAll(mTimers);
	mTimers.clear();
}

QTimer* Script::timer(int milliseconds)
{
	QTimer *result = new QTimer();
	mTimers.append(result);
	result->start(milliseconds);
	return result;
}

void Script::wait(int const &milliseconds)
{
	QEventLoop loop;
	QObject::connect(this, SIGNAL(stopWaiting()), &loop, SLOT(quit()), Qt::DirectConnection);
	QTimer t;
	connect(&t, SIGNAL(timeout()), &loop, SLOT(quit()), Qt::DirectConnection);
	t.start(milliseconds);
	loop.exec();
}

qint64 Script::time() const
{
	return QDateTime::currentMSecsSinceEpoch();
}

void Script::run()
{
	mInEventDrivenMode = true;
}

bool Script::isInEventDrivenMode() const
{
	return mInEventDrivenMode;
}

void Script::quit()
{
	emit quitSignal();
}

void Script::system(QString const &command)
{
	QStringList args{"-c", command};
	QLOG_INFO() << "Running: " << "sh" << args;
	qDebug() << "Running:" << "sh" << args;
	QProcess::startDetached("sh", args);
}
