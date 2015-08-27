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

#include <QtCore/QDateTime>
#include <QtCore/QDir>

#include <trikControl/brickFactory.h>
#include <trikControl/brickInterface.h>
#include <trikKernel/configurer.h>
#include <trikKernel/coreDumping.h>
#include <trikKernel/fileUtils.h>
#include <trikKernel/applicationInitHelper.h>
#include <trikKernel/translationsHelper.h>
#include <trikKernel/parametersHelper.h>
#include <trikKernel/version.h>
#include <trikScriptRunner/trikScriptRunner.h>
#include <trikNetwork/gamepadFactory.h>
#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikNetwork/mailboxInterface.h>

#include <QsLog.h>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	trikKernel::ApplicationInitHelper initHelper(app);

	app.setApplicationName("TrikRun");

	initHelper.parametersHelper().addOption("s", "script"
			, QObject::tr("Script to be executed directly from command line.\n")
					+ QObject::tr("Example: ./trikRun -qws -s \"brick.smile(); script.wait(2000);\""));

	initHelper.parametersHelper().addApplicationDescription(QObject::tr("Runner of JavaScript files."));

	if (!initHelper.parseCommandLine()) {
		return 0;
	}

	initHelper.init();

	QLOG_INFO() << "TrikRun started";

	QScopedPointer<trikControl::BrickInterface> brick(
			trikControl::BrickFactory::create(initHelper.configPath(), initHelper.startDirPath())
			);

	trikKernel::Configurer configurer(initHelper.configPath() + "/system-config.xml"
			, initHelper.configPath() + "/model-config.xml");

	QScopedPointer<trikNetwork::GamepadInterface> gamepad(trikNetwork::GamepadFactory::create(configurer));
	QScopedPointer<trikNetwork::MailboxInterface> mailbox(trikNetwork::MailboxFactory::create(configurer));
	trikScriptRunner::TrikScriptRunner runner(*brick, mailbox.data(), gamepad.data(), initHelper.startDirPath());

	QObject::connect(&runner, SIGNAL(completed(QString, int)), &app, SLOT(quit()));

	if (initHelper.parametersHelper().isSet("s")) {
		runner.run(initHelper.parametersHelper().value("s"));
	} else {
		runner.run(trikKernel::FileUtils::readFromFile(initHelper.parametersHelper().positionalArgs()[0]));
	}

	return app.exec();
}
