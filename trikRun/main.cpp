/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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
#include <QtXml/QDomDocument>

#include <trikControl/brickFactory.h>
#include <trikControl/brickInterface.h>
#include <trikKernel/configurer.h>
#include <trikKernel/coreDumping.h>
#include <trikKernel/fileUtils.h>
#include <trikKernel/loggingHelper.h>
#include <trikScriptRunner/trikScriptRunner.h>
#include <trikNetwork/gamepadFactory.h>
#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikNetwork/mailboxInterface.h>

#include <QsLog.h>

#include "graphicsWidgetHandler.h"

/// For debug
#include "../trikControl/src/usbMSP430Interface.h"
char stmp1[32];

using namespace trikRun;

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

	trikKernel::coreDumping::initCoreDumping(startDirPath);

#ifdef Q_WS_QWS
	QWSServer * const server = QWSServer::instance();
	if (server) {
		server->setCursorVisible(false);
	}
#endif

	trikKernel::LoggingHelper loggingHelper(startDirPath);
	Q_UNUSED(loggingHelper);

	QLOG_INFO() << "TrikRun started";

	QScopedPointer<trikControl::BrickInterface> brick(
			trikControl::BrickFactory::create(*app.thread(), configPath, startDirPath)
	);

	GraphicsWidgetHandler graphicsWidgetHandler;

	QObject::connect(&brick->graphicsWidget(), SIGNAL(showMe(trikKernel::MainWidget&))
			, &graphicsWidgetHandler, SLOT(show(trikKernel::MainWidget&)));

	QObject::connect(&brick->graphicsWidget(), SIGNAL(hideMe()), &graphicsWidgetHandler, SLOT(hide()));

	trikKernel::Configurer configurer(configPath + "/system-config.xml", configPath + "/model-config.xml");
	QScopedPointer<trikNetwork::GamepadInterface> gamepad(trikNetwork::GamepadFactory::create(configurer));
	QScopedPointer<trikNetwork::MailboxInterface> mailbox(trikNetwork::MailboxFactory::create(configurer));
	trikScriptRunner::TrikScriptRunner runner(*brick, mailbox.data(), gamepad.data(), startDirPath);

	QObject::connect(&runner, SIGNAL(completed(QString, int)), &app, SLOT(quit()));

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

    //makeWriteRegPacket(stmp1, 0x26, 0x01, 0x01f4);
    //qDebug() << stmp1;

	return app.exec();
}
