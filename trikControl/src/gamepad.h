/* Copyright 2013 - 2016 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QTimer>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include "deviceInterface.h"
#include "fifo.h"
#include "gamepadInterface.h"

namespace trikControl {

/// Implementation of remote control interface.
class Gamepad : public GamepadInterface, public DeviceInterface
{
	Q_OBJECT

public:
	/// Constructor.
	Gamepad(const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~Gamepad() override;

	Status status() const override;

public slots:
	void reset() override;

	bool buttonWasPressed(int buttonNumber) override;

	bool buttonIsPressed(int buttonNumber) override;

	bool isPadPressed(int pad) const override;

	int padX(int pad) const override;

	int padY(int pad) const override;

	int wheel() const override;

	bool isConnected() const override;

private slots:
	void onNewData(const QString &data);

	void onButtonStateClearTimerTimeout();

private:
	Q_DISABLE_COPY(Gamepad)

	struct PadStatus {
		int x;
		int y;
		bool isPressed;
	};

	void handlePadUp(int padId);

	void handleWheel(int percent);

	void handlePad(int padId, int x, int y);

	void handleButton(int button, int pressed);

	Fifo mUnderlyingFifo;

	QSet<int> mButtonWasPressed;
	QHash<int, bool> mButtonState;

	/// Hack for the fact that gamepad does not send button press events, only releases. We count button as "pressed"
	/// if it was released within 500 milliseconds from this moment. So actually button becomes "pressed" when it is
	/// released and stays pressed for 500 milliseconds.
	QHash<int, QSharedPointer<QTimer>> mButtonStateClearTimers;

	QHash<int, PadStatus> mPads;
	int mWheelPercent = -101;
};

}
