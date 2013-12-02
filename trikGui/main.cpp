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

#include "trikGuiApplication.h"
#include "startWidget.h"

using namespace trikGui;

void printUsage() {
	qDebug() << "Usage: ./trikGui [-qws] [-c <full path to config files>]";
	qDebug() << "Arguments:";
	qDebug() << "  -qws --- start in Qt for Embedded Linux server mode."
				<< "Exactly one GUI application needs to be run as server at a time. trikGui generally"
				<< "shall use this parameter, as in many cases it is the only GUI application on robot.";
	qDebug() << "  -c --- path to a directory where all configs for TRIK runtime are stored. Config files are"
				<< "config.xml (configuration of robot hardware for trikControl library) and wpa-config.xml"
				<< "(configuration of known WiFi networks). Default value for this parameter is current directory."
				<< "Example: ./trikGui -qws -c /home/root/";
}

int main(int argc, char *argv[])
{
	TrikGuiApplication app(argc, argv);

	QTranslator guiTranslator;
	if (!app.arguments().contains("--no-locale")) {
		guiTranslator.load(":/trikGui_ru");
		app.installTranslator(&guiTranslator);
	}

	if (app.arguments().contains("-help")) {
		printUsage();
		return 0;
	}

	QString configPath = "./";
	if (app.arguments().contains("-c")) {
		int const index = app.arguments().indexOf("-c");
		if (app.arguments().count() <= index + 1) {
			printUsage();
			return 1;
		}

		configPath = app.arguments()[index + 1];
		if (configPath.right(1) != "/") {
			configPath += "/";
		}
	}

#ifdef Q_WS_QWS
	QWSServer * const server = QWSServer::instance();
	if (server) {
		server->setCursorVisible(false);
	}
#endif

	StartWidget w(configPath);
	w.show();

	return app.exec();
}
