/* Copyright 2014 CyberTech Labs Ltd.
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
	#include <QtGui/QHBoxLayout>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
#endif

#include "digitSelector.h"

namespace trikGui {

/// Widget that allows to input integer numbers using robot keys.
class NumberSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param defaultValue - number that will be shown in this widget initially.
	/// @param digits - required quantity of selectable digits in a number.
	/// @param separator - a number of digits that shall be separated by '.'. For example, if "separator" value is 3 and
	///        "digits" value is 9, number will be shown as "999.999.999".
	/// @param height - height of digits in pixels.
	/// @param parent - parent of this widget in terms of Qt widges parent/son relations.
	NumberSelectionWidget(int defaultValue, int digits, int separator, int height, QWidget *parent = 0);

	/// Returns current entered number.
	int value() const;

	/// Sets current value. Does not emit valueChanged() signal.
	void setValue(int value);

	/// True, if widget or one of its children has keyboard input focus.
	bool hasFocusInside();

signals:
	/// Emitted when number is changed by user.
	void valueChanged(int newValue);

	/// Widget is not in editing mode and user pressed "up". Event propagation to parent widget does not work for some
	/// reason in this case.
	void upPressed();

	/// Widget is not in editing mode and user pressed "up". Event propagation to parent widget does not work for some
	/// reason in this case.
	void downPressed();

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;

private slots:
	void onValueChanged();

private:
	QList<DigitSelector *> lineEdits() const;

	QHBoxLayout mLayout;
	int mCurrentDigit;
	int mDigits;
	bool mEditingMode;
};

}
