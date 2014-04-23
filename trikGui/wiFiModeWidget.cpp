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

#include "wiFiModeWidget.h"

#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>

#include "wiFiClientWidget.h"
#include "wiFiAPWidget.h"
#include "wiFiInitWidget.h"

using namespace trikGui;

WiFiModeWidget::WiFiModeWidget(QString const &configPath
		, QWidget *parent)
	: QWidget(parent)
	, mConfigPath(configPath)
	, rcReader("/etc/trik/trikrc")
	, mTitle(tr("Choose mode:"))
{
	setWindowState(Qt::WindowFullScreen);

	mModes.addItem(tr("Wi-Fi client"));
	mModes.addItem(tr("Wi-Fi access point"));

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mModes);
	setLayout(&mLayout);

	mModes.selectionModel()->select(
			mModes.model()->index(0, 0)
			, QItemSelectionModel::ClearAndSelect
			);
}

void WiFiModeWidget::exec()
{
	show();
	mEventLoop.exec();
}

QString WiFiModeWidget::menuEntry()
{
	return tr("Network Config");
}

void WiFiModeWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return:
		case Qt::Key_Right: {
			if (mModes.currentItem()->text() == tr("Wi-Fi client")) {
				setMode(client);
			} else {
				setMode(accessPoint);
			}
			break;
		}
		case Qt::Key_Left:
		case Qt::Key_PowerDown:
		case Qt::Key_Meta: {
			mEventLoop.quit();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}

void WiFiModeWidget::setMode(Mode mode)
{
	rcReader.read();

	QString const currentModeText = rcReader.value("trik_wifi_mode");

	Mode currentMode = unknown;
	if (currentModeText == "client") {
		currentMode = client;
	} else if (currentModeText == "ap") {
		currentMode = accessPoint;
	}

	if (currentMode != mode) {
		WiFiInitWidget wiFiInitWidget;
		if (wiFiInitWidget.init(client) == WiFiInitWidget::fail) {
			return;
		}
	}

	switch (mode) {
		case client: {
			WiFiClientWidget wiFiClientWidget(mConfigPath);
			wiFiClientWidget.exec();
			break;
		}
		case accessPoint: {
			WiFiAPWidget wiFiAPWidget;
			wiFiAPWidget.exec();
			break;
		}
		case unknown: {
			qDebug() << "Error: unknown WiFi mode in WiFiModeWidget::setMode()";
			break;
		}
	}
}
