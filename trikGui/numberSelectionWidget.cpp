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

#include <QtGui/QKeyEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QLabel>
#endif

using namespace trikGui;

NumberSelectionWidget::NumberSelectionWidget(int defaultValue, int digits, int separator, int height, QWidget *parent)
	: QWidget(parent)
	, mCurrentDigit(0)
	, mDigits(digits)
	, mEditingMode(false)
{
	mLayout.setSpacing(0);
	mLayout.addStretch();

	int separatorCounter = 1;

	for (int i = 0; i < digits; ++i) {
		auto digitSelector = new DigitSelector(mEditingMode, height);

		mLayout.addWidget(digitSelector);

		connect(digitSelector, &DigitSelector::valueChanged, this, &NumberSelectionWidget::onValueChanged);
		connect(digitSelector, &DigitSelector::upPressed, this, &NumberSelectionWidget::upPressed);
		connect(digitSelector, &DigitSelector::downPressed, this, &NumberSelectionWidget::downPressed);

		if (separator != 0 && separatorCounter % separator == 0 && i != digits - 1) {
			mLayout.addWidget(new QLabel("."));
		}

		++separatorCounter;
	}

	setValue(defaultValue);

	mLayout.addStretch();

	setLayout(&mLayout);
}

bool NumberSelectionWidget::hasFocusInside()
{
	const auto editsList = lineEdits();
	for (int i = 0; i < mDigits; ++i) {
		if (editsList.at(mCurrentDigit)->hasFocus()) {
			return true;
		}
	}

	return hasFocus();
}

int NumberSelectionWidget::value() const
{
	const auto editsList = lineEdits();
	int result = 0;
	for (int i = 0; i < mDigits; ++i) {
		result = result * 10 + editsList.at(i)->text().toInt();
	}

	return result;
}

void NumberSelectionWidget::setValue(int value)
{
	const QString &valueString = QString("%1").arg(value, mDigits, 10, QChar('0'));
	Q_ASSERT(valueString.length() == mDigits);
	const auto editsList = lineEdits();
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
	case Qt::Key_Return: {
		mEditingMode = !mEditingMode;
		update();
		event->accept();
		break;
	}
	default: {
		event->ignore();
		break;
	}
	}
}

void NumberSelectionWidget::focusInEvent(QFocusEvent *event)
{
	Q_UNUSED(event)
	lineEdits().at(mCurrentDigit)->setFocus();
}

QList<DigitSelector *> NumberSelectionWidget::lineEdits() const
{
	QList<DigitSelector *> result;
	for (int i = 0; i < mLayout.count(); ++i) {
		const auto digitSelector = dynamic_cast<DigitSelector *>(mLayout.itemAt(i)->widget());
		if (digitSelector) {
			result.append(digitSelector);
		}
	}

	return result;
}

void NumberSelectionWidget::onValueChanged()
{
	emit valueChanged(value());
}
