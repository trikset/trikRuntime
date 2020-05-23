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

#include <QRandomGenerator>
#include <QsLog.h>

#include "translationsHelper.h"
#include "version.h"
#include "fileUtils.h"
#include "coreDumping.h"
#include "loggingHelper.h"
#include "paths.h"

using namespace trikKernel;

ApplicationInitHelper::ApplicationInitHelper(QCoreApplication &app)
	: mApp(app)
	, mLoggingHelper(new LoggingHelper(Paths::logsPath()))
{
	mApp.setApplicationVersion(trikKernel::version);

	trikKernel::TranslationsHelper::initLocale(app.arguments().contains("--no-locale")
			|| app.arguments().contains("-no-locale"));

	auto *guiApp = qobject_cast<QApplication *>(&app);
	if (guiApp) {
		QFont font(guiApp->font());
		font.setPixelSize(18);
		guiApp->setFont(font);
	}

	mCommandLineParser.addFlag("h", "help", QObject::tr("Print this help text."));
	mCommandLineParser.addFlag("no-locale", "no-locale"
			, QObject::tr("Ignore all locale options and use English language"));

	mCommandLineParser.addOption("c", "config-path"
			, QObject::tr("Path to a directory where all configs for TRIK runtime are stored. Config files are\n"
			"\tsystem-config.xml (system-wide configuration of robot hardware for trikControl library) and\n"
			"\tmodel-config.xml (configuration of current model).")
			);

	mCommandLineParser.addOption("d", "coredump-path"
			, QObject::tr("Path to a directory where core dump will be saved in case of creation.")
			);

#ifdef Q_WS_QWS
	if (!app.arguments().contains("--no-display") && !app.arguments().contains("-no-display")) {
		QWSServer * const server = QWSServer::instance();
		if (server) {
			server->setCursorVisible(false);
		}
	}
#endif
}

ApplicationInitHelper::~ApplicationInitHelper()
{
}

trikKernel::CommandLineParser &ApplicationInitHelper::commandLineParser()
{
	return mCommandLineParser;
}

bool ApplicationInitHelper::parseCommandLine()
{
	if (!mCommandLineParser.process(mApp) || mCommandLineParser.isSet("h")) {
		mCommandLineParser.showHelp();
		return false;
	}

	return true;
}

void ApplicationInitHelper::init()
{
	QString coreDumpPath;
	if (mCommandLineParser.isSet("d")) {
		coreDumpPath = trikKernel::FileUtils::normalizePath(mCommandLineParser.value("d"));
	} else {
		coreDumpPath = Paths::coreDumpPath();
	}

	trikKernel::coreDumping::initCoreDumping(coreDumpPath);

	mConfigPath = Paths::configsPath();
	if (mCommandLineParser.isSet("c")) {
		mConfigPath = trikKernel::FileUtils::normalizePath(mCommandLineParser.value("c"));
	}

	QLOG_INFO() << "====================================================================";
}

QString ApplicationInitHelper::configPath() const
{
	return mConfigPath;
}
