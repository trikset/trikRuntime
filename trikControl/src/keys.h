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
#include <QtCore/QThread>
#include <QtCore/QScopedPointer>
#include <QtCore/QHash>

#include "keysInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class KeysWorker;

/// Implementation of handler for keys on a brick for a real robot.
class Keys : public KeysInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	Keys(const trikKernel::Configurer &configurer, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~Keys() override;

	Status status() const override;

public slots:
	void reset() override;

	bool wasPressed(int code) override;

	bool isPressed(int code) override;

	int buttonCode(bool wait = true) override;

signals:
	/// Emitted when a button's state changed.
	void buttonStateChanged();

private slots:
	void changeButtonState(int code, int value);

private:
	int pressedButton();

	/// Device state, shared with worker object.
	DeviceState mState;

	QScopedPointer<KeysWorker> mKeysWorker;
	QThread mWorkerThread;
	QHash<int, int> mKeysPressed;
};

}
