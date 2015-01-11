/* Copyright 2013 - 2015 CyberTech Labs Ltd.
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
#include <QtXml/QDomDocument>

#include <trikCommunicator/trikCommunicator.h>
#include <trikControl/brickFactory.h>
#include <trikControl/brickInterface.h>
#include <trikKernel/coreDumping.h>
#include <trikKernel/loggingHelper.h>
#include <trikNetwork/gamepadFactory.h>
#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikNetwork/mailboxInterface.h>

#include <QsLog.h>

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

	trikKernel::coreDumping::initCoreDumping(startDirPath);

#ifdef Q_WS_QWS
	QWSServer * const server = QWSServer::instance();
	if (server) {
		server->setCursorVisible(false);
	}
#endif

	trikKernel::LoggingHelper loggingHelper(startDirPath);
	Q_UNUSED(loggingHelper);

	QLOG_INFO() << "TrikServer started on port" << port;

	qDebug() << "Running TrikServer on port" << port;

	QScopedPointer<trikControl::BrickInterface> brick(
			trikControl::BrickFactory::createBrick(*app.thread(), configPath, startDirPath));

	/// @todo: Remove this code to factories or facade, or to objects themselves.
	QDomDocument config("config");

	QFile file(configPath + "config.xml");
	if (!file.open(QIODevice::ReadOnly)) {
		QString const message = "Failed to open config.xml for reading";
		QLOG_FATAL() << message;
		throw message;
	} if (!config.setContent(&file)) {
		file.close();
		QLOG_FATAL() << "config.xml parsing failed";
		throw "config.xml parsing failed";
	}

	file.close();

	QDomElement const root = config.documentElement();
	QScopedPointer<trikNetwork::GamepadInterface> gamepad;
	QScopedPointer<trikNetwork::MailboxInterface> mailbox;

	if (root.elementsByTagName("mailbox").size() > 0
				&& root.elementsByTagName("mailbox").at(0).toElement().attribute("disabled") != "true")
	{
		auto const mailboxElement = root.elementsByTagName("mailbox").at(0).toElement();
		auto const mailboxServerPort = mailboxElement.attribute("port").toInt();
		mailbox.reset(trikNetwork::MailboxFactory::create(mailboxServerPort));
	}

	if (root.elementsByTagName("gamepad").size() > 0
				&& root.elementsByTagName("gamepad").at(0).toElement().attribute("disabled") != "true")
	{
		auto const gamepadElement = root.elementsByTagName("gamepad").at(0).toElement();
		auto const gamepadServerPort = gamepadElement.attribute("port").toInt();
		gamepad.reset(trikNetwork::GamepadFactory::create(gamepadServerPort));
	}

	trikCommunicator::TrikCommunicator communicator(*brick, *mailbox, *gamepad, startDirPath);
	communicator.startServer(port);

	return app.exec();
}
