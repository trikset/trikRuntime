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

#pragma once

#include <QtCore/QSet>

#include "port.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// Hardware implementation of an on-board port.
///
/// This is the future implementation for ports wired directly to the SoC, so it
/// is expected to use Linux facilities (pinctrl states, regulators/load
/// switches, GPIO) instead of the periphery module.
///
/// Currently this class is a no-op: it only tracks the active mode, owner and
/// reference tokens, while start()/stop() do not actually switch pinmux or
/// power. The function that is selected for the port by default keeps working;
/// the real pinmux and power operations are to be performed through trikHal.
///
/// @see PeripheryPort for ports served by the periphery driver.
class HardwarePort final : public Port
{
public:
	HardwarePort(PortConfig config, QList<PortMode> allowedModes,
		trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	QString name() const override;

	QList<PortMode> allowedModes() const override;

	bool allows(PortMode mode) const override;

	PortMode activeMode() const override;

	bool isActive() const override;

	QString owner() const override;

	QString activeDevice() const override;

	quint64 start(PortMode mode, const QString &owner) override;

	void stop(quint64 token) override;

private:
	PortConfig mConfig;
	QList<PortMode> mAllowedModes;
	// Reserved for the future pinmux/power implementation; unused while
	// start()/stop() are no-ops.
	trikHal::HardwareAbstractionInterface &mHardwareAbstraction __attribute__((unused));
	PortMode mActiveMode = PortMode::None;
	QString mOwner;
	QSet<quint64> mTokens;
	quint64 mNextToken = 1;
};

}
