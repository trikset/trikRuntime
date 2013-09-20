/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QtWidgets/QApplication>

namespace trikGui {

/// Application main class, to translate events from robot buttons into more convenient events
/// (like presses to Enter and Escape).
class TrikGuiApplication : public QApplication
{
	Q_OBJECT

public:
	/// Constructor with command-line arguments.
	TrikGuiApplication(int &argc, char **argv);

	// Override.
	bool notify(QObject *receiver, QEvent *event);
};

}
