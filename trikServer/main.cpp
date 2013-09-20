/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <trikCommunicator/trikCommunicator.h>

int main(int argc, char *argv[])
{
	int const port = 8888;

	QCoreApplication app(argc, argv);

	qDebug() << "Running TrikServer on port" << port;

	trikCommunicator::TrikCommunicator communicator;
	communicator.listen(port);

	return app.exec();
}
