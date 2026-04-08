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

#include "wiFiInit.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QsLog.h>

using namespace trikGui;

WiFiInit::WiFiInit(QObject *parent) : QObject(parent) {
	connect(&mProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
		&WiFiInit::onProcessFinished);
	connect(&mProcess, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this,
		&WiFiInit::onProcessError);
	connect(&mProcess, &QProcess::errorOccurred, this, &WiFiInit::onProcessError);
}

WiFiInit::~WiFiInit() {
	if (mProcess.state() != QProcess::NotRunning) {
		QLOG_ERROR() << "Destroyng WiFiInitWidget with runnig process. The user "
				"has cancelled the running operation?";
		mProcess.kill();
	}
}

WiFiInit::Result WiFiInit::init(WiFiMode::Mode mode) {
	const QString command = "/etc/trik/set_wifi_mode.sh";
	QStringList arguments;

	switch (mode) {
	case WiFiMode::Mode::AccessPoint: {
		arguments << "ap";
		break;
	}
	case WiFiMode::Mode::Client: {
		arguments << "client";
		break;
	}
	case WiFiMode::Mode::Unknown: {
		QLOG_ERROR() << "Error: unknown WiFi mode in WiFiInitWidget::init()";
		return Result::fail;
	}
	}

	mProcess.start(command, arguments);

	const int result = mEventLoop.exec();

	if (result != 0) {
		return Result::fail;
	}

	return Result::success;
}

void WiFiInit::exit() {
	mProcess.disconnect();
	mProcess.kill();
	mEventLoop.exit(1);
}

void WiFiInit::onProcessFinished(int, QProcess::ExitStatus exitStatus) {
	mProcess.disconnect();

	switch (exitStatus) {
	case QProcess::NormalExit: {
		mEventLoop.exit(0);
		break;
	}
	case QProcess::CrashExit: {
		mEventLoop.exit(1);
		break;
	}
	}
}

void WiFiInit::onProcessError(QProcess::ProcessError error) {
	mProcess.disconnect();
	QLOG_ERROR() << "set_wifi_mode.sh process error: " << error;
	mEventLoop.exit(1);
}
