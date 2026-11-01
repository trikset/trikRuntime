/* Copyright 2026 CyberTech Labs Ltd.
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

#include "virtualKeySender.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QQuickWindow>

using namespace trikGui;

VirtualKeySender::VirtualKeySender(QObject *parent)
	: QObject(parent)
{
}

void VirtualKeySender::sendKeyPress(int keyCode)
{
	auto *window = qobject_cast<QQuickWindow *>(QGuiApplication::focusWindow());
	if (!window) {
		return;
	}

	// Send press then release to emulate a full key tap
	QKeyEvent press{QEvent::KeyPress, keyCode, Qt::NoModifier};
	QCoreApplication::sendEvent(window, &press);

	QKeyEvent release{QEvent::KeyRelease, keyCode, Qt::NoModifier};
	QCoreApplication::sendEvent(window, &release);
}