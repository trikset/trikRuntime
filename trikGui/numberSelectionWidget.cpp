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

#include "numberSelectionWidget.h"

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

#include <trikControl/sensor.h>

using namespace trikGui;

NumberSelectionWidget::NumberSelectionWidget(int defaultValue, int digits, int separator, QWidget *parent)
	: QWidget(parent)
	, mCurrentDigit(0)
	, mDigits(digits)
	, mEditingMode(false)
{
	mLayout.setSpacing(0);
	mLayout.addStretch();

	int separatorCounter = 1;

	for (int i = 0; i < digits; ++i) {
		auto digitSelector = new SpecialLineEdit(mEditingMode);
		digitSelector->setMaximumWidth(18);
		digitSelector->setReadOnly(true);
		mLayout.addWidget(digitSelector);
		if (separator != 0 && separatorCounter % separator == 0 && i != digits - 1) {
			mLayout.addWidget(new QLabel("."));
		}

		++separatorCounter;
	}

	setValue(defaultValue);

	mLayout.addStretch();

	setLayout(&mLayout);

	setStyleSheet("background-color:lightgray");
}

bool NumberSelectionWidget::hasFocusInside()
{
	auto const editsList = lineEdits();
	for (int i = 0; i < mDigits; ++i) {
		if (editsList.at(mCurrentDigit)->hasFocus()) {
			return true;
		}
	}

	return hasFocus();
}

int NumberSelectionWidget::value() const
{
	auto const editsList = lineEdits();
	int result = 0;
	for (int i = 0; i < mDigits; ++i) {
		result = result * 10 + editsList.at(i)->text().toInt();
	}

	return result;
}

void NumberSelectionWidget::setValue(int value)
{
	QString const &valueString = QString("%1").arg(value, mDigits, 10, QChar('0'));
	Q_ASSERT(valueString.length() == mDigits);
	auto const editsList = lineEdits();
	for (int i = 0; i < mDigits; ++i) {
		editsList.at(i)->setText(valueString[i]);
	}
}

void NumberSelectionWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Right: {
		++mCurrentDigit;
		mCurrentDigit %= mDigits;
		lineEdits().at(mCurrentDigit)->setFocus();
		event->accept();
		break;
	}
	case Qt::Key_Left: {
		--mCurrentDigit;
		mCurrentDigit = (mCurrentDigit + mDigits) % mDigits;
		lineEdits().at(mCurrentDigit)->setFocus();
		event->accept();
		break;
	}
	case Qt::Key_Up: {
		if (!mEditingMode) {
			event->ignore();
			break;
		}

		auto const digitSelector = lineEdits().at(mCurrentDigit);
		int digit = digitSelector->text().toInt();
		digit = (digit + 1) % 10;
		digitSelector->setText(QString::number(digit));
		emit valueChanged(value());
		event->accept();
		break;
	}
	case Qt::Key_Down: {
		if (!mEditingMode) {
			event->ignore();
			break;
		}

		auto const digitSelector = lineEdits().at(mCurrentDigit);
		int digit = digitSelector->text().toInt();
		digit = (digit - 1 + 10) % 10;
		digitSelector->setText(QString::number(digit));
		emit valueChanged(value());
		event->accept();
		break;
	}
	case Qt::Key_Return: {
		mEditingMode = !mEditingMode;
		if (!mEditingMode) {
			setStyleSheet("background-color:lightgray");
		} else {
			setStyleSheet("background-color:palegreen");
		}

		event->accept();
		break;
	}
	default:
	{
		event->ignore();
		break;
	}
	}
}

void NumberSelectionWidget::focusInEvent(QFocusEvent *event)
{
	Q_UNUSED(event)
	mLayout.itemAt(mCurrentDigit + 1)->widget()->setFocus();
}

QList<NumberSelectionWidget::SpecialLineEdit *> NumberSelectionWidget::lineEdits() const
{
	QList<NumberSelectionWidget::SpecialLineEdit *> result;
	for (int i = 0; i < mLayout.count(); ++i) {
		auto const digitSelector = dynamic_cast<SpecialLineEdit *>(mLayout.itemAt(i)->widget());
		if (digitSelector) {
			result.append(digitSelector);
		}
	}

	return result;
}
