/* Copyright 2013 Yurii Litvinov
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
	#include <QtGui/QWSServer>
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QDir>

#include <trikKernel/fileUtils.h>
#include <trikControl/brick.h>
#include <trikScriptRunner/trikScriptRunner.h>

void printUsage()
{
	qDebug() << "Usage: trikRun -qws <QtScript file name> [-c <config file name>] [-d <working directory name>]";
	qDebug() << "Usage: trikRun -qws -s \"<your script>\" [-c <config file name>] [-d <working directory name>]";
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QStringList args = app.arguments();

	if (args.count() < 2) {
		printUsage();
		return 1;
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

	trikControl::Brick brick(*app.thread(), configPath, startDirPath);

	trikScriptRunner::TrikScriptRunner runner(brick, startDirPath);
	QObject::connect(&runner, SIGNAL(completed()), &app, SLOT(quit()));

	if (app.arguments().contains("-s")) {
		runner.run(args[app.arguments().indexOf("-s") + 1]);
	} else {
		args.removeAll("-qws");
		if (args.contains("-c")) {
			args.removeAt(args.indexOf("-c") + 1);
			args.removeAll("-c");
		}

		if (args.contains("-d")) {
			args.removeAt(args.indexOf("-d") + 1);
			args.removeAll("-d");
		}

		if (args.count() != 2) {
			printUsage();
			return 1;
		}

		runner.run(trikKernel::FileUtils::readFromFile(args[1]));
	}

	return app.exec();
}
