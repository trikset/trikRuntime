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
#include <QtCore/QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
#else
	#include <QtWidgets/QWidget>
#endif

using namespace trikGui;

TrikGuiApplication::TrikGuiApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
}

bool TrikGuiApplication::notify(QObject *receiver, QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		// Temporary measure for some demo models which do not use QWS to draw their graphics, so we need to update
		// all GUI to remove clutter on a screen.
		foreach (QWidget * const widget, QApplication::allWidgets()) {
			widget->update();
		}

	return QApplication::notify(receiver, event);
}
