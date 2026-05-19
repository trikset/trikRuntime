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
	connect(&mProcess, &QProcess::errorOccurred, this, &WiFiInit::onProcessError);
}

WiFiInit::~WiFiInit() {
	if (mProcess.state() != QProcess::NotRunning) {
		// Should not happen: init() waits for the process via event loop.
		QLOG_ERROR() << "WiFiInit destroyed with running process";
		mProcess.terminate();
		mProcess.waitForFinished(5000);
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
		QLOG_ERROR() << "Error: unknown WiFi mode in WiFiInit::init()";
		return Result::fail;
	}
	}

	mProcess.start(command, arguments);

	// Blocks here, processing Qt events, until onProcessFinished exits the loop.
	// exit() sends SIGTERM but does NOT exit the loop — we keep waiting for
	// sigterm_handler to finish restoring the previous network mode.
	const int code = mEventLoop.exec();
	return static_cast<Result>(code);
}

void WiFiInit::exit() {
	if (mProcess.state() != QProcess::NotRunning) {
		mRestoring = true;
		Q_EMIT restoringChanged();
		// Do NOT exit the event loop — we keep running until sigterm_handler
		// finishes restoring the previous network mode.
		mProcess.terminate();
	}
}

void WiFiInit::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	mProcess.disconnect();

	if (exitStatus == QProcess::CrashExit) {
		mEventLoop.exit(static_cast<int>(Result::fail));
		return;
	}

	// exit codes from set_wifi_mode.sh:
	//   0 — normal success
	//   1 — script error
	//   2 — cancelled by SIGTERM, sigterm_handler restored previous mode
	switch (exitCode) {
	case 0:
		mEventLoop.exit(static_cast<int>(Result::success));
		break;
	case 2:
		mEventLoop.exit(static_cast<int>(Result::cancelled));
		break;
	default:
		mEventLoop.exit(static_cast<int>(Result::fail));
		break;
	}
}

void WiFiInit::onProcessError(QProcess::ProcessError error) {
	mProcess.disconnect();
	QLOG_ERROR() << "set_wifi_mode.sh process error: " << error;
	mEventLoop.exit(static_cast<int>(Result::fail));
}

void WiFiInit::setQmlParent(QObject *parent) {
	setParent(parent);
}
