/* Copyright 2013 - 2014 CuberTech Labs Ltd.
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

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
	#include <QtGui/QWSServer>
#else
	#include <QtWidgets/QApplication>
#endif

#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <trikCommunicator/trikCommunicator.h>
#include <trikControl/brick.h>

#include "QsLog.h"

void printUsage()
{
	qDebug() << "Usage: trikServer [-c <config file name] [-d <working directory name>]";
}

int main(int argc, char *argv[])
{
	int const port = 8888;

	QApplication app(argc, argv);

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

	QString startDirPath = QDir::currentPath();
	if (app.arguments().contains("-d")) {
		int const index = app.arguments().indexOf("-d");
		if (app.arguments().count() <= index + 1) {
			printUsage();
			return 1;
		}

		startDirPath = app.arguments()[index + 1];
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

	int const maxLogSize = 10 * 1024 * 1024;
	QsLogging::Logger::instance().setLoggingLevel(QsLogging::TraceLevel);
	QsLogging::DestinationPtr destination = QsLogging::DestinationFactory::MakeFileDestination(
			startDirPath + "trik.log"
			, QsLogging::EnableLogRotation
			, QsLogging::MaxSizeBytes(maxLogSize)
			, QsLogging::MaxOldLogCount(2)
			, QsLogging::TraceLevel);
	QsLogging::Logger::instance().addDestination(destination);
	QLOG_INFO() << "TrikServer started on port" << port;

	qDebug() << "Running TrikServer on port" << port;

	trikControl::Brick brick(*app.thread(), configPath, startDirPath);

	trikCommunicator::TrikCommunicator communicator(brick, startDirPath);
	communicator.startServer(port);

	return app.exec();
}
