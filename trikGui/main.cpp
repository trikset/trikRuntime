/* Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWSServer>
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtGui/QFont>

#include "trikGuiApplication.h"
#include "backgroundWidget.h"
#include "QsLog.h"

using namespace trikGui;

void printUsage() {
	qDebug() << "Usage: ./trikGui"
				<< "[-qws]"
				<< "[-c <full path to config files>]"
				<< "[-d <full path to a directory with resources>]";
	qDebug() << "Arguments:";
	qDebug() << "  -qws --- start in Qt for Embedded Linux server mode."
				<< "Exactly one GUI application needs to be run as server at a time. trikGui generally"
				<< "shall use this parameter, as in many cases it is the only GUI application on robot.";
	qDebug() << "  -c --- path to a directory where all configs for TRIK runtime are stored. Config files are"
				<< "config.xml (configuration of robot hardware for trikControl library) and wpa-config.xml"
				<< "(configuration of known WiFi networks). Default value for this parameter is current directory."
				<< "Example: ./trikGui -qws -c /home/root/trik/";
	qDebug() << "  -d --- path to a directory where images, example scripts and system.js file are stored."
				<< "Example: ./trikGui -qws -d /home/root/trik/";
}

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i) {
		if (QString(argv[i]).contains("-help")) {
			printUsage();
			return 0;
		}
	}

	if (argc == 1) {
		printUsage();
	}

	TrikGuiApplication app(argc, argv);

	QFont font(app.font());
	font.setPixelSize(18);
	app.setFont(font);

	QTranslator guiTranslator;
	QTranslator scriptRunnerTranslator;
	if (!app.arguments().contains("--no-locale")) {
		guiTranslator.load(":/trikGui_ru");
		scriptRunnerTranslator.load(":/trikScriptRunner_ru");
		app.installTranslator(&guiTranslator);
		app.installTranslator(&scriptRunnerTranslator);
	}

	QString configPath = QDir::currentPath() + "/";
	if (app.arguments().contains("-c")) {
		int const index = app.arguments().indexOf("-c");
		if (app.arguments().count() <= index + 1) {
			printUsage();
			return 1;
		}

		configPath = app.arguments()[index + 1];
		configPath = QDir(configPath).absolutePath();
		if (configPath.right(1) != "/") {
			configPath += "/";
		}
	}

	QString startDirPath = QDir::currentPath();
	if (app.arguments().contains("-d")) {
		int const index = app.arguments().indexOf("-d");
		if (app.arguments().count() <= index + 1) {
			printUsage();
			return 1;
		}

		startDirPath = app.arguments()[index + 1];
		startDirPath = QDir(startDirPath).absolutePath();
	}

	if (startDirPath.right(1) != "/") {
		startDirPath += "/";
	}

#ifdef Q_WS_QWS
	QWSServer * const server = QWSServer::instance();
	if (server) {
		server->setCursorVisible(false);
	}
#endif

	if (QDir::current().exists("scripts")) {
		QDir::setCurrent("scripts");
	}

	BackgroundWidget w(configPath, startDirPath);
	w.show();

	int const maxLogSize = 10 * 1024 * 1024;
	QsLogging::Logger::instance().setLoggingLevel(QsLogging::TraceLevel);
	QsLogging::Logger::instance().addDestination(QsLogging::DestinationPtr(QsLogging::DestinationFactory::MakeFileDestination(startDirPath + "trik.log", QsLogging::EnableLogRotation
			, QsLogging::MaxSizeBytes(maxLogSize), QsLogging::MaxOldLogCount(2), QsLogging::TraceLevel)));
	QLOG_INFO() << "TrikGui started";

	return app.exec();
}
