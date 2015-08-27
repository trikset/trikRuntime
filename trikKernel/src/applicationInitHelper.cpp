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

#include "applicationInitHelper.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtGui/QFont>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
	#include <QtGui/QWSServer>
#else
	#include <QtWidgets/QApplication>
#endif

#include <QsLog.h>

#include "loggingHelper.h"
#include "translationsHelper.h"
#include "version.h"
#include "fileUtils.h"
#include "coreDumping.h"

using namespace trikKernel;

ApplicationInitHelper::ApplicationInitHelper(QCoreApplication &app)
	: mApp(app)
{
	qsrand(QDateTime::currentMSecsSinceEpoch());
	mApp.setApplicationVersion(trikKernel::version);

	trikKernel::TranslationsHelper::initLocale(app.arguments().contains("--no-locale")
			|| app.arguments().contains("-no-locale"));

	QApplication *guiApp = dynamic_cast<QApplication *>(&app);
	if (guiApp) {
		QFont font(guiApp->font());
		font.setPixelSize(18);
		guiApp->setFont(font);
	}

	trikKernel::LoggingHelper loggingHelper(".");
	Q_UNUSED(loggingHelper);

	mParametersHelper.addFlag("h", "help", QObject::tr("Print this help text."));
	mParametersHelper.addFlag("no-locale", "no-locale"
			, QObject::tr("Ignore all locale options and use English language"));

	mParametersHelper.addOption("c", "config-path"
			, QObject::tr("Path to a directory where all configs for TRIK runtime are stored. Config files are\n"
			"config.xml (configuration of robot hardware for trikControl library) and wpa-config.xml\n"
			"(configuration of known WiFi networks). Default value for this parameter is current directory.")
			);

	mParametersHelper.addOption("d", "resources-directory"
			, QObject::tr("Path to a directory where images, example scripts and system.js file are stored.")
			);

#ifdef Q_WS_QWS
	QWSServer * const server = QWSServer::instance();
	if (server) {
		server->setCursorVisible(false);
	}
#endif
}

trikKernel::ParametersHelper &ApplicationInitHelper::parametersHelper()
{
	return mParametersHelper;
}

bool ApplicationInitHelper::parseCommandLine()
{
	if (!mParametersHelper.process(mApp) || mParametersHelper.isSet("h")) {
		mParametersHelper.showHelp();
		return false;
	}

	return true;
}

void ApplicationInitHelper::init()
{
	trikKernel::coreDumping::initCoreDumping(".");

	mConfigPath = QDir::currentPath() + "/";
	if (mParametersHelper.isSet("c")) {
		mConfigPath = trikKernel::FileUtils::normalizePath(mParametersHelper.value("c"));
	}

	mStartDirPath = QDir::currentPath();
	if (mParametersHelper.isSet("d")) {
		mStartDirPath = trikKernel::FileUtils::normalizePath(mParametersHelper.value("d"));
	}

	QLOG_INFO() << "====================================================================";
}

QString ApplicationInitHelper::configPath() const
{
	return mConfigPath;
}

QString ApplicationInitHelper::startDirPath() const
{
	return mStartDirPath;
}
