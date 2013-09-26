/* Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#include "trikGuiApplication.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

TrikGuiApplication::TrikGuiApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
}

bool TrikGuiApplication::notify(QObject *receiver, QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent * const keyEvent = dynamic_cast<QKeyEvent *>(event);
		if (keyEvent != NULL) {
			switch (keyEvent->key()) {
				case Qt::Key_F2: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Meta, Qt::NoModifier);
					break;
				}
				case Qt::Key_F3: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
					break;
				}
				case Qt::Key_F4: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
					break;
				}
				case Qt::Key_F5: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
					break;
				}
				case Qt::Key_F6: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
					break;
				}
				case Qt::Key_F7: {
					*keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
					break;
				}
			}
		}
	}

	return QApplication::notify(receiver, event);
}
