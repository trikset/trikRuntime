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
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include <QtCore/QTimer>
#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>

#include <trikKernel/configurer.h>
#include <trikKernel/deinitializationHelper.h>
#include <trikKernel/fileUtils.h>
#include <trikKernel/applicationInitHelper.h>
#include <trikKernel/paths.h>
#include <trikControl/brickFactory.h>
#include <trikControl/brickInterface.h>
#include <trikScriptRunner/trikScriptRunner.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikNetwork/mailboxInterface.h>

#include <QsLog.h>

int main(int argc, char *argv[])
{
	QStringList params;
	for (int i = 1; i < argc; ++i) {
		params << QString(argv[i]);
	}

	QScopedPointer<QCoreApplication> app;

	if (params.contains("--no-display") || params.contains("-no-display")) {
		app.reset(new QCoreApplication(argc, argv));
	} else {
		app.reset(new QApplication(argc, argv));
	}

	app->setApplicationName("TrikRun");

	// RAII-style code to ensure that after brick gets destroyed there will be an event loop that cleans it up.
	trikKernel::DeinitializationHelper helper;
	Q_UNUSED(helper);

	trikKernel::ApplicationInitHelper initHelper(*app);

	initHelper.commandLineParser().addPositionalArgument("file", QObject::tr("File with script to execute")
			+ " " + QObject::tr("(optional of -s option is specified)"));

	initHelper.commandLineParser().addOption("s", "script"
			, QObject::tr("Script to be executed directly from command line.") + "\n"
					+ QObject::tr("\tExample: ./trikRun -qws -s \"brick.smile(); script.wait(2000);\""));

	initHelper.commandLineParser().addFlag("no-display", "no-display"
			, QObject::tr("Disable display support. When this flag is active, trikRun can work without QWS or even "
						  "physical display"));

	initHelper.commandLineParser().addApplicationDescription(QObject::tr("Runner of JavaScript files."));

	if (!initHelper.parseCommandLine()) {
		return 0;
	}

	initHelper.init();

	QLOG_INFO() << "TrikRun started";

	const auto run = [&](const QString &script) {
		QScopedPointer<trikControl::BrickInterface> brick(
				trikControl::BrickFactory::create(initHelper.configPath(), trikKernel::Paths::mediaPath())
				);

		trikKernel::Configurer configurer(initHelper.configPath() + "/system-config.xml"
				, initHelper.configPath() + "/model-config.xml");

		QScopedPointer<trikNetwork::MailboxInterface> mailbox(trikNetwork::MailboxFactory::create(configurer));
		trikScriptRunner::TrikScriptRunner result(*brick, mailbox.data());

		QObject::connect(&result, SIGNAL(completed(QString, int)), app.data(), SLOT(quit()));
		result.run(script);
		return app->exec();
	};

	if (initHelper.commandLineParser().isSet("s")) {
		return run(initHelper.commandLineParser().value("s"));
	} else {
		const QStringList positionalArgs = initHelper.commandLineParser().positionalArgs();
		if (positionalArgs.size() == 1) {
			return run(trikKernel::FileUtils::readFromFile(positionalArgs[0]));
		} else {
			initHelper.commandLineParser().showHelp();
			return 1;
		}
	}
}
