/* Copyright 2016 Mikhail Kita
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
	#include <QtGui/QWidget>
	#include <QtGui/QLabel>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QListWidget>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListWidget>
#endif

#include <QtCore/QString>

#include "trikGuiDialog.h"
#include "controller.h"

namespace trikGui {

/// Widget which allows to choose commands for robot.
class CommandsListWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param controller - reference to controller object which provides access to low-level functionality.
	/// @param name - name of command which was specified before.
	/// @param parent - parent of this widget in Qt object hierarchy.
	CommandsListWidget(Controller &controller, const QString &name, QWidget *parent = 0);

	/// Returns menu entry for this widget.
	static QString menuEntry();

	void renewFocus() override;

	/// Returns string with new name of command.
	QString value() const;

	/// Returns string with part of script.
	QString script() const;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	/// Defines behaviour for commands "Motor Forward" and "Motor Backward".
	void motorBehaviour();

	/// Defines behaviour for commands related to sensors.
	/// @param port - name of port for given sensor.
	/// @param isEncoder - flag which shows whether it is encoder.
	void sensorBehaviour(const QString &port, bool isEncoder);

private:
	QVBoxLayout mLayout;
	QLabel mTitle;
	QListWidget mCommands;
	Controller &mController;

	/// Contains string with current name of command.
	QString mValue;

	/// Contains part of script.
	QString mScript;
};

}
