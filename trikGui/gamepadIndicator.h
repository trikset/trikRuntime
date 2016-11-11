/* Copyright 2016 Anna Kudryashova
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

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QLabel>
#endif

#include <controller.h>

namespace trikGui {

/// A label that shows gamepad connection status.
class GamepadIndicator : public QLabel
{
	Q_OBJECT
public:
	/// @param controller is used to get the current gamepad info
	explicit GamepadIndicator(Controller &controller, QWidget *parent = 0);

public slots:
	/// Updates the status to 'connect'.
	void setOn();

	/// Updates the status to 'disconnect'.
	void setOff();

	/// Requests connection info from the controller and updates the status.
	void updateStatus();

	/// Updates the status according to connected parameters.
	void connected(bool connected);

private:
	QTimer mUpdateTimer;
	Controller &mController;
};

}
