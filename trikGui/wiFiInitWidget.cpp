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

#include "wiFiInitWidget.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include "messageBox.h"

using namespace trikGui;

WiFiInitWidget::WiFiInitWidget(QWidget *parent)
	: QWidget(parent)
	, mInitMessage(tr("Network initialization in process"))
	, mWaitMessage(tr("Please wait"))
	, mBreakMessage(tr("Press any key for break"))
{
	setWindowState(Qt::WindowFullScreen);

	mInitMessage.setAlignment(Qt::AlignCenter);
	mWaitMessage.setAlignment(Qt::AlignCenter);

	mLayout.addWidget(&mInitMessage);
	mLayout.addWidget(&mWaitMessage);
	mLayout.addWidget(&mBreakMessage);
	setLayout(&mLayout);

	connect(&mProcess, SIGNAL(finished(int)), SLOT(onProcessFinished(int, QProcess::ExitStatus)));
}

WiFiInitWidget::Result WiFiInitWidget::init(WiFiModeWidget::Mode mode)
{
	QString command = "/etc/set_wifi_mode.sh";
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
			qDebug() << "Error: unknown WiFi mode in WiFiInitWidget::init()";
			return fail;
		}
	}

	mProcess.start(command, arguments);

	show();

	if (mEventLoop.exec() == 0) {
		return success;
	} else {
		return fail;
	}

	close();
}

void WiFiInitWidget::keyPressEvent(QKeyEvent *)
{
	disconnect(&mProcess);
	mProcess.kill();
	mEventLoop.exit(1);
}

void WiFiInitWidget::onProcessFinished(int, QProcess::ExitStatus exitStatus)
{
	switch (exitStatus) {
		case QProcess::NormalExit: {
			mEventLoop.exit(0);
			break;
		}
		case QProcess::CrashExit: {
			MessageBox messageBox;
			messageBox.exec(tr("Network initialization failed"));

			mEventLoop.exit(1);
			break;
		}
	}
}
