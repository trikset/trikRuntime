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

#include <QProcess>

#include "wiFiIndicator.h"

#include "trikKernel/paths.h"

using namespace trikGui;

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
		case WiFiModeWidget::client:
			if (mController.wiFi().statusResult().connected) {
				QProcess strengthProcess;
				strengthProcess.start("/usr/share/trik/wifi_strength.sh");
				strengthProcess.waitForFinished();
				bool res;
				auto strength = QString(strengthProcess.readAllStandardOutput()).toInt(&res);
				if (res) {
					if (strength >= -110 && strength <= -90) {
						setLowStrength();
					} else if (strength > -90 && strength <= -55) {
						setMediumStrength();
					} else if (strength > -55 && strength <= -40) {
						setHighStrength();
					}
				} else {
					setOn();
				}
			} else {
				setOff();
			}
			break;
		case WiFiModeWidget::accessPoint:
			setAPOn();
			break;
		case WiFiModeWidget::unknown:
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

	changeMode(wiFiMode);
}
