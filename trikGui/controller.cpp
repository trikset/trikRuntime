/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "controller.h"

#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

using namespace trikGui;

int const communicatorPort = 8888;

Controller::Controller()
	: mCommunicator(mScriptRunner)
{
	mCommunicator.listen(communicatorPort);
}

void Controller::runFile(QString const &filePath)
{
	QFileInfo const fileInfo(filePath);
	if (fileInfo.suffix() == "qts") {
		mScriptRunner.runFromFile(fileInfo.canonicalFilePath());
	} else if (fileInfo.isExecutable()) {
		QProcess::startDetached(filePath);
	}
}
