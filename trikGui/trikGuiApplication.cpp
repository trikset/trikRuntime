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
#include <QtCore/QProcess>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
#else
	#include <QtWidgets/QWidget>
#endif

#include "backgroundWidget.h"

using namespace trikGui;

TrikGuiApplication::TrikGuiApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
	connect(&mPowerButtonPressedTimer, SIGNAL(timeout()), this, SLOT(shutdown()));
}

bool TrikGuiApplication::notify(QObject *receiver, QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		refreshWidgets();
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_PowerOff && keyEvent->isAutoRepeat() && !mPowerButtonPressedTimer.isActive()) {
			mPowerButtonPressedTimer.start(3000);
		}
	} else if (event->type() == QEvent::KeyRelease) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_PowerOff && !keyEvent->isAutoRepeat()) {
			mPowerButtonPressedTimer.stop();
		}
	}

	return QApplication::notify(receiver, event);
}

void TrikGuiApplication::refreshWidgets()
{
	if (dynamic_cast<BackgroundWidget *>(QApplication::activeWindow())) {
		for (const auto widget : QApplication::allWidgets()) {
			widget->update();
		}
	}
}

void TrikGuiApplication::shutdown()
{
	if (!mIsShuttingDown) {
		QProcess::startDetached("/bin/sh", {"-c", "halt"});
		mIsShuttingDown = true;
	}
}
