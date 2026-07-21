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

#include "controller.h"

namespace trikGui {

/// Window that shows information about currently executed script.
class RunningCode : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param programName - name of a script that is executed.
	/// @param controller - reference to object providing access to low-level functionality.
	/// @param parent - parent of a widget in terms of Qt parent-child widget relations.
	explicit RunningCode(Controller &controller, QObject *parent = nullptr);

	/// Associates running widget with a new script.
	void setProgram(const QString &programName, int scriptId);

	/// Returns an id of the script which information running widget is showing at the moment.
	int scriptId() const;

	Q_INVOKABLE void abortScript();

private Q_SLOTS:
	void showRunningCode(const QString &fileName, int scriptId);
	void hideRunningCode(int scriptId);
	void hideScript();
	/// Shows given error message on a widget.
	void showError(const QString &error, int scriptId);

private:
	QString mProgramName;
	QString mError;
	Controller &mController;
	int mScriptId{-1};
Q_SIGNALS:
	/// Emitted when it is necessary to show the widget of running script
	void showRunningCodeComponent(const QString &programName);
	/// Emitted when it is necessary to show the widget with error of running script
	void showErrorRunningCodeComponent(const QString &error);
	/// Emitted when it is necessary to hide the widget of running script
	void hideRunningCodeComponent();
	/// Emitted when it is necessary to hide the widget of running script
	void hideScriptComponent();
};

}
