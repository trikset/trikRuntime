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
#else
	#include <QtWidgets/QApplication>
#endif

#include <QtCore/QDebug>

#include <trikKernel/applicationInitHelper.h>
#include <trikKernel/configurer.h>
#include <trikKernel/paths.h>
#include <trikCommunicator/trikCommunicator.h>
#include <trikControl/brickFactory.h>
#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikNetwork/mailboxInterface.h>

#include <QsLog.h>

int main(int argc, char *argv[])
{
	const int port = 8888;

	QApplication app(argc, argv);
	app.setApplicationName("TrikServer");
	trikKernel::ApplicationInitHelper initHelper(app);

	if (!initHelper.parseCommandLine()) {
		return 0;
	}

	initHelper.init();

	QLOG_INFO() << "TrikServer started on port" << port;
	qDebug() << "Running TrikServer on port" << port;

	QScopedPointer<trikControl::BrickInterface> brick(trikControl::BrickFactory::create(
			initHelper.configPath(), trikKernel::Paths::mediaPath()));

	trikKernel::Configurer configurer(
			initHelper.configPath() + "/system-config.xml"
			, initHelper.configPath() + "/model-config.xml"
			);

	QScopedPointer<trikNetwork::MailboxInterface> mailbox(trikNetwork::MailboxFactory::create(configurer));

	trikCommunicator::TrikCommunicator communicator(*brick, mailbox.data());
	communicator.startServer(port);

	return app.exec();
}
