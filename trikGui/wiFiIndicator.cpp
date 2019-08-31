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
#include "trikWiFi/networkStructs.h"

using namespace trikGui;
using namespace trikWiFi;

WiFiIndicator::WiFiIndicator(Controller &controller, QWidget *parent)
	: QLabel(parent)
	, mController(controller)
{
	connect(&mController, &Controller::wiFiConnected, this, &WiFiIndicator::setOn);
	connect(&mController, &Controller::wiFiDisconnected, this, &WiFiIndicator::setOff);

	updateStatus();
	connect(&mUpdateTimer, &QTimer::timeout, this, &WiFiIndicator::updateStatus);
	mUpdateTimer.start(7000);
}

void WiFiIndicator::setOn()
{
	if (mMode == WiFiModeWidget::Mode::client) {
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

void WiFiIndicator::setLowStrength()
{
	QPixmap icon("://resources/wifi-signal-low.png");
	setPixmap(icon);
}

void WiFiIndicator::setMediumStrength()
{
	QPixmap icon("://resources/wifi-signal-medium.png");
	setPixmap(icon);
}

void WiFiIndicator::setHighStrength()
{
	QPixmap icon("://resources/wifi-signal-high.png");
	setPixmap(icon);
}

void WiFiIndicator::changeMode(WiFiModeWidget::Mode mode)
{
	mMode = mode;
	switch (mode) {
		case WiFiModeWidget::Mode::client:
			if (mController.wiFi().statusResult().connected) {
				switch (mController.wiFi().signalStrength()) {
					case SignalStrength::undefined:
						setOn();
						break;
					case SignalStrength::low:
						setLowStrength();
						break;
					case SignalStrength::medium:
						setMediumStrength();
						break;
					case SignalStrength::high:
						setHighStrength();
						break;
				}
			} else {
				setOff();
			}
			break;
		case WiFiModeWidget::Mode::accessPoint:
			setAPOn();
			break;
		case WiFiModeWidget::Mode::unknown:
			setOff();
	}
}

void WiFiIndicator::updateStatus()
{
	trikKernel::RcReader rcReader(trikKernel::Paths::trikRcName());
	rcReader.read();

	WiFiModeWidget::Mode wiFiMode;
	if (rcReader.value("trik_wifi_mode") == "ap") {
		wiFiMode = WiFiModeWidget::Mode::accessPoint;
	} else if (rcReader.value("trik_wifi_mode") == "client") {
		wiFiMode = WiFiModeWidget::Mode::client;
	} else {
		wiFiMode = WiFiModeWidget::Mode::unknown;
	}

	changeMode(wiFiMode);
}
