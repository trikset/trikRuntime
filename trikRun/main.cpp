/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
