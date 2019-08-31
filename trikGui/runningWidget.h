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

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
#endif

#include <QtCore/QList>
#include <QtCore/QString>

#include "controller.h"
#include "mainWidget.h"

namespace trikGui {

/// Window that shows information about currently executed script.
class RunningWidget : public MainWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param programName - name of a script that is executed.
	/// @param controller - reference to object providing access to low-level functionality.
	/// @param parent - parent of a widget in terms of Qt parent-child widget relations.
	explicit RunningWidget(Controller &controller, QWidget *parent = 0);

	/// Shows given error message on a widget.
	void showError(const QString &error, int scriptId);

	/// Associates running widget with a new script.
	void setProgram(const QString &programName, int scriptId);

	/// Returns an id of the script which information running widget is showing at the moment.
	int scriptId() const;

	void renewFocus() override;

signals:
	/// Emitted when running widget wants to hide itself.
	void hideMe(int scriptId);

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	QVBoxLayout mLayout;
	QLabel mStatusLabel;
	QLabel mAbortLabel;
	Controller &mController;
	int mScriptId { -1 };
};

}
