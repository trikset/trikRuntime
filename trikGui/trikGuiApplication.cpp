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
#include <QtWidgets/QWidget>

#include "backgroundWidget.h"

using namespace trikGui;

TrikGuiApplication::TrikGuiApplication(int &argc, char **argv)
	: QApplication(argc, argv)
{
	connect(&mPowerButtonPressedTimer, &QTimer::timeout, this, &TrikGuiApplication::shutdownSoon);
	connect(&mShutdownDelayTimer, &QTimer::timeout, this, &TrikGuiApplication::shutdown);
	mPowerButtonPressedTimer.setSingleShot(true);
	mShutdownDelayTimer.setSingleShot(true);
}

static bool isTrikPowerOffKey(Qt::Key key) {
	return key == Qt::Key_PowerOff || ( key == Qt::Key_W && (QApplication::keyboardModifiers() & Qt::ControlModifier));
}

bool TrikGuiApplication::notify(QObject *receiver, QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
		if (isTrikPowerOffKey(static_cast<Qt::Key>(keyEvent->key()))) {
			if (keyEvent->isAutoRepeat()) {
				//	if (!mPowerButtonPressedTimer.isActive()) {
				//	mPowerButtonPressedTimer.start(2000);
				//	}
			} else {
				if (!mPowerButtonPressedTimer.isActive()) {
					mPowerButtonPressedTimer.start(1500);
				}
				mIsShutdownRequested = true;
				refreshWidgets(); // refresh display if not auto-repeat
			}
			static QKeyEvent evntKeyPowerOff(QEvent::KeyPress, Qt::Key_PowerOff, Qt::NoModifier);
			event = &evntKeyPowerOff;
		}
	} else if (event->type() == QEvent::KeyRelease) {
		QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
		if (isTrikPowerOffKey(static_cast<Qt::Key>(keyEvent->key()))) {
			if (!keyEvent->isAutoRepeat()) {
				mIsShutdownRequested = false;
//				if (mPowerButtonPressedTimer.isActive()) {
//					mPowerButtonPressedTimer.stop();
//				}
			} else {
			}
			static QKeyEvent evntKeyPowerOff(QEvent::KeyRelease, Qt::Key_PowerOff, Qt::NoModifier);
			event = &evntKeyPowerOff;
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
	if(!mIsShutdownRequested) {
		setStyleSheet(mSavedStyleSheet);
		return;
	}

	QProcess::startDetached("/sbin/shutdown", {"-h", "-P", "now" });
	QCoreApplication::quit();
}

void TrikGuiApplication::shutdownSoon()
{
	if(mShutdownDelayTimer.isActive() || !mIsShutdownRequested) {
		return;
	}

	mSavedStyleSheet = styleSheet();
	setStyleSheet(mSavedStyleSheet + " QWidget { background-color:red; } ");
	mShutdownDelayTimer.start(1500);
}
