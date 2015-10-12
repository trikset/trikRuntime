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

#include "wiFiIndicator.h"

#include "trikKernel/paths.h"

using namespace trikGui;

WiFiIndicator::WiFiIndicator(Controller &controller, QWidget *parent)
	: QLabel(parent)
	, mController(controller)
{
	connect(&mController, SIGNAL(wiFiConnected()), this, SLOT(setOn()));
	connect(&mController, SIGNAL(wiFiDisconnected()), this, SLOT(setOff()));

	updateStatus();
	connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	mUpdateTimer.start(7000);
}

void WiFiIndicator::setOn()
{
	if (mMode == WiFiModeWidget::client) {
		QPixmap icon("://resources/wifion.png");
		setPixmap(icon);
	}
}

void WiFiIndicator::setOff()
{
	QPixmap icon("://resources/wifioff.png");
	setPixmap(icon);
}

void WiFiIndicator::setAPOn()
{
	QPixmap icon("://resources/wifipoint.png");
	setPixmap(icon);
}

void WiFiIndicator::changeMode(WiFiModeWidget::Mode mode, bool connected)
{
	mMode = mode;

	switch (mode) {
		case WiFiModeWidget::client:
			if (connected) {
				setOn();
			} else {
				setOff();
			}
			break;
		case WiFiModeWidget::accessPoint:
			setAPOn();
			break;
		default:
			setOff();
	}
}

void WiFiIndicator::updateStatus()
{
	trikKernel::RcReader rcReader(trikKernel::Paths::trikRcName());
	rcReader.read();
	WiFiModeWidget::Mode wiFiMode;
	if (rcReader.value("trik_wifi_mode") == "ap") {
		wiFiMode = WiFiModeWidget::accessPoint;
	} else if (rcReader.value("trik_wifi_mode") == "client") {
		wiFiMode = WiFiModeWidget::client;
	} else {
		wiFiMode = WiFiModeWidget::unknown;
	}

	changeMode(wiFiMode, wiFiMode == WiFiModeWidget::client && mController.wiFi().status().connected);
}
