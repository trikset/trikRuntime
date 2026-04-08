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

WiFiIndicator::WiFiIndicator(Controller &controller, QObject *parent) : QObject(parent), mController(controller) {
	connect(&mController, &Controller::wiFiConnected, this, &WiFiIndicator::updateStatus);
	connect(&mController, &Controller::wiFiDisconnected, this, &WiFiIndicator::updateStatus);

	updateStatus();
	connect(&mUpdateTimer, &QTimer::timeout, this, &WiFiIndicator::updateStatus);
	mUpdateTimer.start(5000);
}

void WiFiIndicator::updateStatus() {

	trikKernel::RcReader rcReader(trikKernel::Paths::trikRcName());
	rcReader.read();

	if (rcReader.value("trik_wifi_mode") == "ap") {
		mMode = WiFiMode::Mode::AccessPoint;
	} else if (rcReader.value("trik_wifi_mode") == "client") {
		mMode = WiFiMode::Mode::Client;
	} else {
		mMode = WiFiMode::Mode::Unknown;
	}

	Q_EMIT modeChanged();
}

WiFiMode::Mode WiFiIndicator::mode() { return mMode; }

bool WiFiIndicator::isConnected() {
	mController.wiFi().statusRequest();
	return mController.wiFi().statusResult().connected;
}
