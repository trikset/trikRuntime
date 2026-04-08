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

#include "wiFiMode.h"

#include <trikKernel/paths.h>
#include <QsLog.h>

#include "wiFiAP.h"
#include "wiFiClient.h"
#include "wiFiInit.h"
#include <QQmlContext>


using namespace trikGui;

WiFiMode::WiFiMode(trikWiFi::TrikWiFi &wiFi, QQmlApplicationEngine *engine, QObject *parent)
	: QObject(parent), mWiFi(wiFi), mEngine(engine), mRcReader(trikKernel::Paths::trikRcName()) {}

void WiFiMode::createWiFiClient() {
	WiFiClient *wiFiClient = new WiFiClient(mWiFi, this);
	mEngine->rootContext()->setContextProperty("WiFiClientServer", wiFiClient);
	wiFiClient->scanWiFi();
}

void WiFiMode::createWiFiAP() {
	WiFiAP *wiFiAP = new WiFiAP(this);
	mEngine->rootContext()->setContextProperty("WiFiAPServer", wiFiAP);
}

void WiFiMode::setMode(Mode mode) {
	mRcReader.read();
	const QString currentModeText = mRcReader.value("trik_wifi_mode");

	Mode currentMode = Mode::Unknown;
	if (currentModeText == "client") {
		currentMode = Mode::Client;
	} else if (currentModeText == "ap") {
		currentMode = Mode::AccessPoint;
	}

	if (currentMode != mode) {
		WiFiInit wiFiInit;
		mEngine->rootContext()->setContextProperty("WiFiInitServer", &wiFiInit);
		mInitStatus = "start";
		Q_EMIT initStatusChanged();
		if (wiFiInit.init(mode) == WiFiInit::Result::fail) {
			QLOG_ERROR() << "Failed to init WiFi in mode" << currentModeText;
			mInitStatus = "error";
			Q_EMIT initStatusChanged();
			return;
		}
	}

	switch (mode) {
	case Mode::Client: {
		if (currentMode != mode) {
			mWiFi.reinit();
		}
		createWiFiClient();
		mInitStatus = "WiFiClient";
		Q_EMIT initStatusChanged();
		break;
	}
	case Mode::AccessPoint: {
		mWiFi.dispose();
		createWiFiAP();
		mInitStatus = "WiFiAP";
		Q_EMIT initStatusChanged();
		break;
	}
	case Mode::Unknown: {
		QLOG_ERROR() << "Error: unknown WiFi mode in WiFiModeWidget::setMode()";
		break;
	}
	}
}

QString WiFiMode::initStatus() { return mInitStatus; }

void WiFiMode::setQmlParent(QObject *parent) { setParent(parent); }

WiFiMode::Mode WiFiMode::currentMode() {
	mRcReader.read();
	const QString currentModeText = mRcReader.value("trik_wifi_mode");

	if (currentModeText == "client") {
		return Mode::Client;
	} else if (currentModeText == "ap") {
		return Mode::AccessPoint;
	} else {
		return Mode::Unknown;
	}
}
