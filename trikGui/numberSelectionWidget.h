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
#include <QtGui/QKeyEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QHBoxLayout>
	#include <QtGui/QLabel>
	#include <QtGui/QLineEdit>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QLineEdit>
#endif

namespace trikControl {
	class Sensor;
}

namespace trikGui {

class NumberSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	NumberSelectionWidget(int defaultValue, int digits, int separator, QWidget *parent = 0);

	int value() const;
	void setValue(int value);

	bool hasFocusInside();

signals:
	void valueChanged(int newValue);

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;

private:
	class SpecialLineEdit : public QLineEdit
	{
	public:
		SpecialLineEdit(bool &editingMode)
			: mEditingMode(editingMode)
		{
		}

	protected:
		void keyPressEvent(QKeyEvent *event)
		{
			event->ignore();
		}

	private:
		bool &mEditingMode;
	};

	QList<SpecialLineEdit *> lineEdits() const;

	QHBoxLayout mLayout;
	int mCurrentDigit;
	int mDigits;
	bool mEditingMode;
};

}
