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
#include "numberSelectionWidget.h"
#include "connectButton.h"

namespace trikGui {

/// Widget which allows to change settings of robot command.
class CommandSettingsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param title - information for user in the top of the page.
	/// @param digits - amount of digits which selector should show.
	/// @param parent - parent of this widget in Qt object hierarchy.
	CommandSettingsWidget(const QString &title, const int digits, QWidget *parent = 0);

	/// Returns menu entry for this widget.
	static QString menuEntry();

	void renewFocus() override;

	/// Returns value of selector.
	int value() const;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	/// Called by child widgets when they need to move input focus up or down.
	void focus();

private:
	QEventLoop mEventLoop;
	QVBoxLayout mLayout;
	QLabel mTitle;

	/// Selector that allows to select some digits.
	NumberSelectionWidget mValueSelector;

	/// Button that returns focus to the previous screen.
	ConnectButton mContinueButton;
};

}
