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
	#include <QtGui/QLineEdit>
#else
	#include <QtWidgets/QLineEdit>
#endif

namespace trikGui {

/// Widget that allows to select one digit by Up/Down keys and can switch states between editing and inactive (where
/// Up and Down keys are ignored).
class DigitSelector : public QLineEdit
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param editingMode - reference to mode flag. If this flag is true, number is currently edited and shall be
	///        increased/decreased by up/down buttons.
	/// @param height - height of a digit.
	DigitSelector(bool &editingMode, int height);

signals:
	/// Digit is changed by user.
	void valueChanged(int newValue);

	/// Widget is not in editing mode and user pressed "up". Event propagation to parent widget does not work for some
	/// reason in this case.
	void upPressed();

	/// Widget is not in editing mode and user pressed "down". Event propagation to parent widget does not work for some
	/// reason in this case.
	void downPressed();

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

private:
	/// Reference to editing mode flag from parent widget.
	bool &mEditingMode;
};

}
