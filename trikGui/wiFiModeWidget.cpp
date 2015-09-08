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

#include <trikKernel/paths.h>
#include <QsLog.h>

#include "wiFiClientWidget.h"
#include "wiFiAPWidget.h"
#include "wiFiInitWidget.h"

using namespace trikGui;

WiFiModeWidget::WiFiModeWidget(const QString &configPath
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mConfigPath(configPath)
	, mRcReader(trikKernel::Paths::trikRcName())
	, mTitle(tr("Choose mode:"))
{
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

QString WiFiModeWidget::menuEntry()
{
	return tr("Network Config");
}

void WiFiModeWidget::renewFocus()
{
	mModes.setFocus();
}

void WiFiModeWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return: {
			if (mModes.currentItem()->text() == tr("Wi-Fi client")) {
				setMode(client);
			} else {
				setMode(accessPoint);
			}
			break;
		}
		default: {
			TrikGuiDialog::keyPressEvent(event);
			break;
		}
	}
}

void WiFiModeWidget::setMode(Mode mode)
{
	mRcReader.read();

	const QString currentModeText = mRcReader.value("trik_wifi_mode");

	Mode currentMode = unknown;
	if (currentModeText == "client") {
		currentMode = client;
	} else if (currentModeText == "ap") {
		currentMode = accessPoint;
	}

	if (currentMode != mode) {
		WiFiInitWidget wiFiInitWidget;
		emit newWidget(wiFiInitWidget);
		if (wiFiInitWidget.init(mode) == WiFiInitWidget::fail) {
			return;
		}
	}

	int returnValue = 0;

	switch (mode) {
		case client: {
			WiFiClientWidget wiFiClientWidget(mConfigPath);
			emit newWidget(wiFiClientWidget);
			returnValue = wiFiClientWidget.exec();
			break;
		}
		case accessPoint: {
			WiFiAPWidget wiFiAPWidget;
			emit newWidget(wiFiAPWidget);
			returnValue = wiFiAPWidget.exec();
			break;
		}
		case unknown: {
			QLOG_ERROR() << "Error: unknown WiFi mode in WiFiModeWidget::setMode()";
			qDebug() << "Error: unknown WiFi mode in WiFiModeWidget::setMode()";
			break;
		}
	}

	if (returnValue == 1) {
		goHome();
	}
}
