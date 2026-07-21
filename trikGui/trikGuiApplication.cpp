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
 * limitations under the License. */

#include "trikGuiApplication.h"

#include <QKeyEvent>
#include <QProcess>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QExposeEvent>

using namespace trikGui;

TrikGuiApplication::TrikGuiApplication(int &argc, char **argv)
	: QGuiApplication(argc, argv)
{
	connect(&mPowerButtonPressedTimer, &QTimer::timeout, this, &TrikGuiApplication::shutdownSoon);
	connect(&mShutdownDelayTimer, &QTimer::timeout, this, &TrikGuiApplication::shutdown);
	mPowerButtonPressedTimer.setSingleShot(true);
	mShutdownDelayTimer.setSingleShot(true);
	installEventFilter(this);
}

static bool isTrikPowerOffKey(const QKeyEvent *ke)
{
	return ke->key() == Qt::Key_PowerOff
		|| (ke->key() == Qt::Key_W && (ke->modifiers() & Qt::ControlModifier));
}

bool TrikGuiApplication::eventFilter(QObject *obj, QEvent *event)
{
	Q_UNUSED(obj);

	if (event->type() == QEvent::KeyPress) {
		auto *keyEvent = static_cast<QKeyEvent *>(event);
		if (!keyEvent->isAutoRepeat() && isTrikPowerOffKey(keyEvent)) {
			if (!mPowerButtonPressedTimer.isActive()) {
				mPowerButtonPressedTimer.start(1500);
			}
			mIsShutdownRequested = true;
			forceRepaint();
		}
	} else if (event->type() == QEvent::KeyRelease) {
		auto *keyEvent = static_cast<QKeyEvent *>(event);
		if (!keyEvent->isAutoRepeat() && isTrikPowerOffKey(keyEvent)) {
			mIsShutdownRequested = false;
		}
	}
	return false;
}

void TrikGuiApplication::shutdownSoon()
{
	if (mShutdownDelayTimer.isActive() || !mIsShutdownRequested) {
		return;
	}

	Q_EMIT showPowerConfirm();
	mShutdownDelayTimer.start(1500);
}

void TrikGuiApplication::shutdown()
{
	if (!mIsShutdownRequested) {
		Q_EMIT hidePowerConfirm();
		return;
	}

	QProcess::startDetached("/sbin/shutdown", {"-h", "-P", "now"});
	QCoreApplication::quit();
}

// Quick fix, we need the ability to redraw the GUI
// if we have occupied the display buffer, for example, with video sensors.
void TrikGuiApplication::forceRepaint()
{
	for (auto *window : QGuiApplication::allWindows()) {
		if (auto *quickWindow = qobject_cast<QQuickWindow *>(window)) {
			QRect fullRect(0, 0, quickWindow->width(), quickWindow->height());
			QCoreApplication::postEvent(quickWindow, new QExposeEvent(QRegion(fullRect)));
		}
	}
}
