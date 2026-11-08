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

#include "peripheryPort.h"

#include <utility>

#include "peripheryDriverInterface.h"

using namespace trikControl;

PeripheryPort::PeripheryPort(PortConfig config, QList<PortMode> allowedModes, PeripheryDriverInterface &driver)
	: mConfig(std::move(config))
	, mAllowedModes(std::move(allowedModes))
	, mDriver(driver)
{
}

QString PeripheryPort::name() const
{
	return mConfig.name;
}

QList<PortMode> PeripheryPort::allowedModes() const
{
	return mAllowedModes;
}

bool PeripheryPort::allows(PortMode mode) const
{
	return mAllowedModes.contains(mode);
}

PortMode PeripheryPort::activeMode() const
{
	return mActiveMode;
}

bool PeripheryPort::isActive() const
{
	return !mTokens.isEmpty();
}

QString PeripheryPort::owner() const
{
	return mOwner;
}

QString PeripheryPort::activeDevice() const
{
	if (mActiveMode == PortMode::None) {
		return {};
	}

	return mConfig.modes.value(mActiveMode).device;
}

quint64 PeripheryPort::start(PortMode mode, const QString &owner)
{
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

void PeripheryPort::stop(quint64 token)
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
