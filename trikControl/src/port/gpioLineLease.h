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

/// RAII helper for a legacy sysfs GPIO line.
///
/// Exports the line when needed, sets its direction (and the initial value for
/// outputs) and unexports it on destruction only when this object did the
/// export.
///
/// The line is identified either by the RK GPIO name ("GPIO4_C5") or by the
/// sysfs value file (/sys/class/gpio/gpioN/value). Paths that are not a sysfs
/// GPIO value file (for example test fixtures under /tmp) are accepted as a
/// no-op and reported as valid.
class GpioLineLease
{
	Q_DISABLE_COPY(GpioLineLease)

public:
	/// Direction of the GPIO line.
	enum class Direction {
		/// Input.
		In,
		/// Output.
		Out,
	};

	/// Creates an empty lease that owns no line.
	GpioLineLease() = default;

	/// Resolves @p valueFile, exports the line if needed and configures it.
	///
	/// @param valueFile - RK GPIO name ("GPIO4_C5") or sysfs value file.
	/// @param direction - requested line direction.
	/// @param initialValue - value written for an output line; ignored for input.
	/// @param hardwareAbstraction - HAL used to write sysfs files.
	GpioLineLease(const QString &valueFile, Direction direction, bool initialValue,
		const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	/// Takes the line from @p other; @p other becomes empty and no longer
	/// unexports the line.
	GpioLineLease(GpioLineLease &&other) noexcept;

	/// Unexports the currently held line (if this object exported it), then
	/// takes it from @p other.
	GpioLineLease &operator=(GpioLineLease &&other) noexcept;

	/// Unexports the held line when this object exported it.
	~GpioLineLease();

	/// Returns true when the line was prepared, or when the path is not a sysfs
	/// GPIO and the lease is intentionally a no-op.
	explicit operator bool() const;

	/// Returns the sysfs value file path the lease refers to.
	QString valueFile() const;

	/// Releases the line early, unexporting it when this object exported it.
	void release();

private:
	/// Unexports the line if this object exported it.
	void unexport();

	/// Clears the lease fields without touching sysfs.
	void clear();

	/// HAL used to write export/unexport; null for an empty lease.
	const trikHal::HardwareAbstractionInterface *mHardwareAbstraction = nullptr;

	/// Sysfs GPIO number as a string, empty for a no-op lease.
	QString mNumber;

	/// Sysfs value file path the lease refers to.
	QString mValueFile;

	/// True when the line was exported by this object and must be unexported.
	bool mExportedByUs = false;

	/// True when the lease is usable (or intentionally a no-op).
	bool mValid = false;
};

}
