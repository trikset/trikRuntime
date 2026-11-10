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

#include "hardwarePort.h"

#include <utility>

#include <trikHal/hardwareAbstractionInterface.h>

using namespace trikControl;

HardwarePort::HardwarePort(PortConfig config, QList<PortMode> allowedModes,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mConfig(std::move(config))
	, mAllowedModes(std::move(allowedModes))
	, mHardwareAbstraction(hardwareAbstraction)
{
}

QString HardwarePort::name() const
{
	return mConfig.name;
}

QList<PortMode> HardwarePort::allowedModes() const
{
	return mAllowedModes;
}

bool HardwarePort::allows(PortMode mode) const
{
	return mAllowedModes.contains(mode);
}

PortMode HardwarePort::activeMode() const
{
	return mActiveMode;
}

bool HardwarePort::isActive() const
{
	return !mTokens.isEmpty();
}

QString HardwarePort::owner() const
{
	return mOwner;
}

QString HardwarePort::activeDevice() const
{
	if (mActiveMode == PortMode::None) {
		return {};
	}

	return mConfig.modes.value(mActiveMode).device;
}

quint64 HardwarePort::start(PortMode mode, const QString &owner)
{
	// Will be used when pinmux/power switching is implemented.
	Q_UNUSED(mHardwareAbstraction);

	if (mode == PortMode::None || !allows(mode)) {
		return 0;
	}

	// Re-acquiring the same mode is a no-op for the hardware: keep the
	// current pinctrl/power state and just add another reference.
	if (mActiveMode == mode) {
		const auto token = mNextToken++;
		mTokens.insert(token);
		return token;
	}

	if (!mTokens.isEmpty()) {
		return 0;
	}

	mActiveMode = mode;
	mOwner = owner;
	const auto token = mNextToken++;
	mTokens.insert(token);

	return token;
}

void HardwarePort::stop(quint64 token)
{
	if (token == 0 || !mTokens.remove(token)) {
		return;
	}

	if (!mTokens.isEmpty()) {
		return;
	}

	mActiveMode = PortMode::None;
	mOwner.clear();
}
