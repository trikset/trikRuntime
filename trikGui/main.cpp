/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QtCore/QCoreApplication>

#include "trikGuiApplication.h"
#include "startWidget.h"

using namespace trikGui;

int main(int argc, char *argv[])
{
	TrikGuiApplication a(argc, argv);
	StartWidget w;
	w.show();

	return a.exec();
}
