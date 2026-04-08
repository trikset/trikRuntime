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

#ifdef DESKTOP
#include "wiFiModeMock.h"
#endif

#include "batteryIndicator.h"
#include "communicationSettings.h"
#include "fileManager.h"
#include "gamepadIndicator.h"
#include "information.h"
#include "languageSelection.h"
#include "mainMenuManager.h"
#include "modeManager.h"
#include "network.h"
#include "openSocketIndicator.h"
#include "testingManager.h"
#include "wiFiIndicator.h"
#include "trikGuiApplication.h"
#include <QProcess>
#include <QQmlContext>
#include "fileManager.h"
#include "runningCode.h"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickItem>
#include <QExposeEvent>

using namespace trikGui;
MainMenuManager::MainMenuManager(const QString &configPath, QQmlApplicationEngine *engine, QObject *parent)
	: QObject(parent), mController(configPath), mEngine(engine)
	, mFileManagerRoot(SystemSettings::FileManagerRootType::ScriptsDir) {
	qmlRegisterTypesAndRevisions<MainMenuManager>("com.trikGui", 1);
	qmlRegisterTypesAndRevisions<TestingManager>("com.trikGui", 1);
	qmlRegisterTypesAndRevisions<WiFiMode>("com.trikGui", 1);
	qmlRegisterSingletonInstance<MainMenuManager>("com.trikGui", 1, 0, "MainMenuManager", this);

	auto *runningCode = new RunningCode(mController, this);
	qmlRegisterSingletonInstance<RunningCode>("com.trikGui", 1, 0, "RunningCode", runningCode);

	auto *network = new Network(this);
	qmlRegisterSingletonInstance<Network>("com.trikGui", 1, 0, "Network", network);

	auto *batteryIndicator = new BatteryIndicator(mController.brick(), this);
	qmlRegisterSingletonInstance<BatteryIndicator>("com.trikGui", 1, 0, "BatteryIndicator", batteryIndicator);

	auto *wiFiIndicator = new WiFiIndicator(mController, this);
	qmlRegisterSingletonInstance<WiFiIndicator>("com.trikGui", 1, 0, "WiFiIndicator", wiFiIndicator);

	auto *gamepadIndicator = new GamepadIndicator(mController, this);
	qmlRegisterSingletonInstance<GamepadIndicator>("com.trikGui", 1, 0, "GamepadIndicator", gamepadIndicator);

	auto *mailboxIndicator = new OpenSocketIndicator(mController.mailbox()->isConnected(), this);
	connect(&mController, &Controller::mailboxStatusChanged, mailboxIndicator, &OpenSocketIndicator::changeStatus);
	qmlRegisterSingletonInstance<OpenSocketIndicator>("com.trikGui", 1, 0, "MailboxIndicator", mailboxIndicator);

	auto *communicatorIndicator =
	    new OpenSocketIndicator(mController.communicatorConnectionStatus(), this);
	connect(&mController, &Controller::communicatorStatusChanged, communicatorIndicator,
		&OpenSocketIndicator::changeStatus);
	qmlRegisterSingletonInstance<OpenSocketIndicator>("com.trikGui", 1, 0, "CommunicatorIndicator",
							  communicatorIndicator);

	auto *trikApp = qobject_cast<TrikGuiApplication *>(qApp);
	connect(trikApp, &TrikGuiApplication::showPowerConfirm, this, &MainMenuManager::showPowerConfirm);
	connect(trikApp, &TrikGuiApplication::hidePowerConfirm, this, &MainMenuManager::hidePowerConfirm);

	mController.brick().playTone(2000, 150);
}

MainMenuManager::~MainMenuManager() {}

void MainMenuManager::createApp(AppType appType) {
	switch (appType) {
	case AppType::Files: {
		FileManager *fileManager = new FileManager(mController, mFileManagerRoot, this);
		mEngine->rootContext()->setContextProperty("FileManagerServer", fileManager);
		break;
	}
	case AppType::Testing: {
		QProcess::startDetached("/etc/trik/init-ov7670-320x240.sh", {"0"});
		QProcess::startDetached("/etc/trik/init-ov7670-320x240.sh", {"1"});
		TestingManager *testingManager = new TestingManager(mController, mEngine, this);
		mEngine->rootContext()->setContextProperty("TestingManager", testingManager);
		break;
	}
	case AppType::Network: {
#ifndef DESKTOP
		WiFiMode *wiFiMode = new WiFiMode(mController.wiFi(), mEngine, this);
#else
		WiFiModeMock *wiFiMode = new WiFiModeMock(mEngine, this);
#endif
		mEngine->rootContext()->setContextProperty("WiFiModeServer", wiFiMode);
		break;
	}
	case AppType::CommSettings: {
		if (mController.mailbox()) {
			CommunicationSettings *communicationSettings =
			    new CommunicationSettings(*mController.mailbox(), this);
			mEngine->rootContext()->setContextProperty("CommunicationSettingsServer",
								   communicationSettings);
		} else {
			Q_ASSERT(!"Mailbox is disabled but "
				  "commmunications widget "
				  "still tries "
				  "to be shown");
		}
		break;
	}
	case AppType::Language: {
		auto *languageSelection = new LanguageSelection(mEngine, this);
		mEngine->rootContext()->setContextProperty("LanguageSelection", languageSelection);
		break;
	}
	case AppType::SystemSettings: {
		auto *systemSettings = new SystemSettings(mFileManagerRoot, this);
		connect(systemSettings, &SystemSettings::currentFilesDirPath, this,
			&MainMenuManager::changeFileManagerRoot);
		mEngine->rootContext()->setContextProperty("SystemSettingsComponent", systemSettings);
		break;
	}
	case AppType::AppearanceMode: {
		auto *modeManager = new ModeManager(mEngine, this);
		mEngine->rootContext()->setContextProperty("ModeManager", modeManager);
		break;
	}
	case AppType::Information: {
		auto *information = new Information(this);
		mEngine->rootContext()->setContextProperty("SystemInformation", information);
		break;
	}
	default:
		break;
	}
}

void MainMenuManager::changeFileManagerRoot(SystemSettings::FileManagerRootType const &path) {
	mFileManagerRoot = path;
	Q_EMIT fileManagerRootChanged(mFileManagerRoot);
}
