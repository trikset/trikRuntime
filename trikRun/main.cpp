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
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <trikScriptRunner/trikScriptRunner.h>

void printUsage()
{
	qDebug() << "Usage: trikRun <QtScript file name> [-c <config file name]>";
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QStringList const args = app.arguments();

	if (args.count() != 2) {
		printUsage();
		return 1;
	}

	QString const scriptFileName = args[1];

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

	trikScriptRunner::TrikScriptRunner runner(configPath);
	QObject::connect(&runner, SIGNAL(completed()), &app, SLOT(quit()));
	runner.runFromFile(scriptFileName);

	return app.exec();
}
