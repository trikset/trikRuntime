/* Copyright 2015 CyberTech Labs Ltd.
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

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>

#include "keys.h"

using namespace trikRuntimeKiller;

void printUsage()
{
	qDebug() << "Usage: trikRuntimeKiller";
	qDebug() << "Kills trikGui process when 'Down', 'Enter' and 'Escape' buttons are pressed simultaneously";
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	const QStringList args = app.arguments();

	if (args.count() > 1) {
		printUsage();
		return 1;
	}

	Keys keys;
	Q_UNUSED(keys)

	return app.exec();
}
