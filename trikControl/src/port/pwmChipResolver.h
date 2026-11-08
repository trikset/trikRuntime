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

namespace trikControl {

/// Resolves an "export" config value ("fe700020.pwm" or "fe700020.pwm:line")
/// to the path of the chip "export" file.
///
/// A missing ":line" means channel 0.
///
/// @param exportSpec - controller device name (or full path) with an optional
///        channel suffix.
/// @param line - receives the channel number; set to 0 when no suffix is given.
/// @return path to /sys/class/pwm/pwmchipN/export, or an empty string when the
///         spec is malformed or the controller cannot be found.
QString findPwmExportFile(const QString &exportSpec, int &line);

/// Finds the /sys/class/pwm/pwmchipN directory belonging to a PWM controller
/// identified by its stable platform device name (for example "fe700020.pwm").
///
/// The pwmchipN number is allocated dynamically by the kernel, so it must not be
/// hardcoded. Every chip exposes a "device" symlink to its parent platform
/// device; this helper matches that link against the requested name.
///
/// @param deviceName - stable platform device name ("fe700020.pwm") or a full
///        path to it.
/// @return path to the pwmchipN directory, or an empty string when not found.
QString findPwmChipByDevice(const QString &deviceName);

}
