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

#include <QtCore/QString>

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// RAII handle for an exported PWM channel.
///
/// Resolves the "export" config value ("fe700020.pwm" or "fe700020.pwm:line")
/// to a pwmchipN directory, exports the channel when it is not exported yet and
/// unexports it on destruction only when this object did the export.
///
/// The lease only manages the existence of the channel. The device itself
/// configures period, duty cycle and enable files under channelDir().
class PwmChannelLease
{
	Q_DISABLE_COPY(PwmChannelLease)

public:
	/// Creates an empty lease that owns no channel.
	PwmChannelLease() = default;

	/// Resolves @p exportSpec and exports the channel when needed.
	///
	/// @param exportSpec - stable controller name with an optional ":line"
	///        suffix, for example "fe700020.pwm" or "fe700020.pwm:1".
	/// @param hardwareAbstraction - HAL used to write the export/unexport files.
	PwmChannelLease(const QString &exportSpec, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	/// Takes the exported channel from @p other; @p other becomes empty and no
	/// longer unexports the channel.
	PwmChannelLease(PwmChannelLease &&other) noexcept;

	/// Unexports the currently held channel (if this object exported it), then
	/// takes it from @p other.
	PwmChannelLease &operator=(PwmChannelLease &&other) noexcept;

	/// Unexports the held channel when this object exported it.
	~PwmChannelLease();

	/// Returns true when the channel was resolved and exported.
	explicit operator bool() const;

	/// Releases the channel early, unexporting it when this object exported it.
	void release();

	/// Returns the channel number (the M in pwmM).
	int line() const;

	/// Returns the channel directory (/sys/class/pwm/pwmchipN/pwmM), or an
	/// empty string for an empty lease.
	QString channelDir() const;

private:
	/// Unexports the channel if this object exported it.
	void unexport();

	/// Clears the lease fields without touching sysfs.
	void clear();

	/// HAL used to write export/unexport; null for an empty lease.
	const trikHal::HardwareAbstractionInterface *mHardwareAbstraction = nullptr;

	/// Resolved /sys/class/pwm/pwmchipN directory.
	QString mChipDir;

	/// Exported channel number, -1 for an empty lease.
	int mLine = -1;

	/// True when the channel was exported by this object and must be unexported.
	bool mExportedByUs = false;
};

}
