/* Copyright 2024 Daniel Chehade.
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

#include "wiFiModeMock.h"
#include <QtGui/QKeyEvent>
#include <QsLog.h>
#include <trikKernel/paths.h>
#include "wiFiAPMock.h"
#include "wiFiClientMock.h"
#include "wiFiInit.h"
#include <QQmlContext>

using namespace trikGui;

WiFiModeMock::WiFiModeMock(QQmlApplicationEngine *engine, QObject *parent)
	: QObject(parent), mEngine(engine) {}

void WiFiModeMock::createWiFiClient() {
	WiFiClientMock *wiFiClient = new WiFiClientMock(this);
	mEngine->rootContext()->setContextProperty("WiFiClientServer", wiFiClient);
	wiFiClient->scanWiFi();
}

void WiFiModeMock::createWiFiAP() {
	WiFiAPMock *wiFiAP = new WiFiAPMock(this);
	mEngine->rootContext()->setContextProperty("WiFiAPServer", wiFiAP);
}

void WiFiModeMock::setMode(Mode mode) {
	if (mCurrentMode != mode) {
		WiFiInit wiFiInit;
		mEngine->rootContext()->setContextProperty("WiFiInitServer", &wiFiInit);
		mInitStatus = "start";
		Q_EMIT initStatusChanged();
	}
	mCurrentMode = mode;
	switch (mode) {
	case Mode::Client: {
		createWiFiClient();
		mInitStatus = "WiFiClient";
		Q_EMIT initStatusChanged();
		break;
	}
	case Mode::AccessPoint: {
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

QString WiFiModeMock::initStatus() { return mInitStatus; }

void WiFiModeMock::setQmlParent(QObject *parent) { setParent(parent); }

WiFiModeMock::Mode WiFiModeMock::currentMode() { return mCurrentMode; }
