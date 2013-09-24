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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <trikScriptRunner/trikScriptRunner.h>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList const args = app.arguments();

	if (args.count() != 2) {
		qDebug() << "Usage: trikRun <QtScript file name>";
		return 1;
	}

	QString const scriptFileName = args[1];

	trikScriptRunner::TrikScriptRunner runner;
	runner.runFromFile(scriptFileName, true);

	return 0;
}
