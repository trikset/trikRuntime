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

#include "wiFiInitWidget.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtGui/QKeyEvent>

#include "trikGuiMessageBox.h"

#include <QsLog.h>

using namespace trikGui;

WiFiInitWidget::WiFiInitWidget(QWidget *parent)
	: MainWidget(parent)
	, mInitMessage(tr("Network initialization" "\n"
			"in process"))
	, mWaitMessage(tr("Please wait"))
	, mBreakMessage(tr("Press Escape" "\n"
			"for break"))
{
	setWindowState(Qt::WindowFullScreen);

	mInitMessage.setAlignment(Qt::AlignCenter);
	mWaitMessage.setAlignment(Qt::AlignCenter);
	mBreakMessage.setAlignment(Qt::AlignCenter);

	mLayout.addWidget(&mInitMessage);
	mLayout.addWidget(&mWaitMessage);
	mLayout.addWidget(&mBreakMessage);
	setLayout(&mLayout);

	connect(&mProcess, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(onProcessFinished(int, QProcess::ExitStatus)));
	connect(&mProcess, SIGNAL(error(QProcess::ProcessError)), SLOT(onProcessError(QProcess::ProcessError)));
}

WiFiInitWidget::Result WiFiInitWidget::init(WiFiModeWidget::Mode mode)
{
	QString command = "/etc/trik/set_wifi_mode.sh";
	QStringList arguments;

	switch (mode) {
		case WiFiModeWidget::accessPoint: {
			arguments << "ap";
			break;
		}
		case WiFiModeWidget::client: {
			arguments << "client";
			break;
		}
		case WiFiModeWidget::unknown: {
			QLOG_ERROR() << "Error: unknown WiFi mode in WiFiInitWidget::init()";
			return fail;
		}
	}

	mProcess.start(command, arguments);

	show();

	const int result = mEventLoop.exec();

	close();

	if (result != 0) {
		return fail;
	}

	return success;
}

void WiFiInitWidget::renewFocus()
{
	setFocus();
}

void WiFiInitWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape) {
		disconnect(&mProcess);
		mProcess.kill();
		mEventLoop.exit(1);
	} else {
		QWidget::keyPressEvent(event);
	}
}

void WiFiInitWidget::onProcessFinished(int, QProcess::ExitStatus exitStatus)
{
	disconnect(&mProcess);

	switch (exitStatus) {
		case QProcess::NormalExit: {
			mEventLoop.exit(0);
			break;
		}
		case QProcess::CrashExit: {
			TrikGuiMessageBox messageBox;
			emit newWidget(messageBox);
			messageBox.exec(tr("Network initialization" "\n"
					"failed"));

			mEventLoop.exit(1);
			break;
		}
	}
}

void WiFiInitWidget::onProcessError(QProcess::ProcessError error)
{
	disconnect(&mProcess);

	QLOG_ERROR() << "set_wifi_mode.sh process error: " << error;

	if (mProcess.state() != QProcess::NotRunning) {
		mProcess.kill();
	}

	mEventLoop.exit(1);
}
