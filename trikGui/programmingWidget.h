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
	#include <QtGui/QMessageBox>
	#include <QtGui/QScrollArea>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListWidget>
	#include <QtWidgets/QMessageBox>
	#include <QtWidgets/QScrollArea>
#endif

#include <QtCore/QString>

#include "trikGuiDialog.h"
#include "controller.h"
#include "connectButton.h"
#include "scriptHolder.h"

namespace trikGui {

/// Widget which allows to write simple programs via the robot interface.
class ProgrammingWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param controller - reference to controller object which provides access to low-level functionality.
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit ProgrammingWidget(Controller &controller, QWidget *parent = 0);

	/// Returns menu entry for this widget.
	static QString menuEntry();

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	/// Adds command to the execution list.
	void addCommand();

private:
	QLabel mTitle;
	QVBoxLayout mLayout;
	QListWidget mCommands;
	Controller &mController;

	/// Counter of empty commands.
	int mEmptyCommandsCounter;

	/// Contains string with main part of program for robot.
	const QString mScript =
		"var __interpretation_started_timestamp__;\n"
		"var pi = 3.14159265;\n\n"
		"var main = function()\n"
		"{\n"
		"    __interpretation_started_timestamp__ = Date.now();\n";
};

}
