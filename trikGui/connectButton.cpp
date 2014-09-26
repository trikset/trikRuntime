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

#include "connectButton.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

using namespace trikGui;

void ConnectButton::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up: {
		emit upPressed();
		return;
	}
	case Qt::Key_Down: {
		emit downPressed();
		return;
	}
	default: {
		event->ignore();
		break;
	}
	}
}

void ConnectButton::paintEvent(QPaintEvent *event)
{
	QPushButton::paintEvent(event);

	if (hasFocus()) {
		QPainter painter(this);
		QPen pen = painter.pen();
		pen.setColor(Qt::green);
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawRect(rect().adjusted(1, 1, -1, -1));
	}
}
