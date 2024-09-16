/* Copyright 2013 - 2015 Roman Kurbatov and CyberTech Labs Ltd.
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

#include <QtCore/QDir>

#include <trikKernel/applicationInitHelper.h>
#include <trikKernel/deinitializationHelper.h>
#include <QsLog.h>

#include "trikGuiApplication.h"
#include "backgroundWidget.h"

using namespace trikGui;

int main(int argc, char *argv[])
{
	TrikGuiApplication app(argc, argv);

	trikKernel::DeinitializationHelper helper;
	Q_UNUSED(helper);

	trikKernel::ApplicationInitHelper initHelper(app);

	app.setApplicationName("TrikGui");

	QFile File(":/resources/stylesheet.qss");
	File.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(File.readAll());
	app.setStyleSheet(styleSheet);

	initHelper.commandLineParser().addApplicationDescription(
		QObject::tr("Graphical user interface, TRIK Studio runtime environment and script runner of a robot")
		);

	if (!initHelper.parseCommandLine()) {
		return 0;
	}

	initHelper.init();

	if (QDir::current().exists("scripts")) {
		QDir::setCurrent("scripts");
	}

	QLOG_INFO() << "TrikGui started";

	BackgroundWidget w(initHelper.configPath());
	w.show();

	return app.exec();
}
