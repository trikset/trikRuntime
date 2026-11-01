/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QSet>
#include <QtCore/QWaitCondition>

#include "keysInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// Implementation of handler for keys on a brick for a real robot.
/// Reads key events from QML virtual key panel instead of physical GPIO buttons.
class Keys : public KeysInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	Keys(const trikKernel::Configurer &configurer, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~Keys() override;

	Status status() const override;

public Q_SLOTS:
	void reset() override;

	bool wasPressed(int code) override;

	bool isPressed(int code) override;

	int buttonCode(bool wait = true) override;

	/// Called from QML when a virtual key is pressed.
	Q_INVOKABLE void emulateKeyPress(int code) override;

Q_SIGNALS:
	/// Emitted when a button's state changed.
	void buttonStateChanged();

private:
	int pressedButton();

	DeviceState mState;
	QHash<int, int> mKeysPressed;
	QSet<int> mWasPressed;
	QMutex mMutex;
	QWaitCondition mWaitCondition;
};

}
