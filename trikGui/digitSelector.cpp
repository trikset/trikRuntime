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

#include "digitSelector.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

using namespace trikGui;

DigitSelector::DigitSelector(bool &editingMode, int height)
	: mEditingMode(editingMode)
{
	setMaximumWidth(height);
	setReadOnly(true);

	QFont font = this->font();
	font.setPixelSize(height);
	setFont(font);
}

void DigitSelector::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up: {
		if (!mEditingMode) {
			emit upPressed();
			return;
		}

		int digit = text().toInt();
		digit = (digit + 1) % 10;
		setText(QString::number(digit));
		emit valueChanged(digit);
		return;
	}
	case Qt::Key_Down: {
		if (!mEditingMode) {
			emit downPressed();
			return;
		}

		int digit = text().toInt();
		digit = (digit - 1 + 10) % 10;
		setText(QString::number(digit));
		emit valueChanged(digit);
		return;
	}
	default:
	{
		event->ignore();
		break;
	}
	}
}

void DigitSelector::paintEvent(QPaintEvent *event)
{
	QLineEdit::paintEvent(event);

	if (hasFocus()) {
		QPainter painter(this);
		QColor const color = mEditingMode ? Qt::green : Qt::red;
		QPen pen = painter.pen();
		pen.setColor(color);
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawRect(rect().adjusted(1, 1, -1, -1));
	}
}
