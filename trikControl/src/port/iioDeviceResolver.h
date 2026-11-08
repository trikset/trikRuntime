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

/// Finds an IIO device by its unique device-tree label.
///
/// The "iio:deviceN" number is allocated by the kernel dynamically, so it must
/// not be hardcoded. IIO core exposes the DT "label" property of the device in
/// the "label" sysfs file, so a stable label can be used instead of the number.
///
/// An absolute path is accepted as an explicit device directory (used by tests
/// and for bring-up, where the sysfs tree may not exist yet).
///
/// @param label - DT label, for example "B1" or "adc-mux", or an absolute path
///        to the IIO device directory.
/// @return path to /sys/bus/iio/devices/iio:deviceN, or an empty string when no
///         device with this label is found.
QString findIioDeviceByLabel(const QString &label);

/// Resolves a file of an IIO channel.
///
/// @p channel is resolved in the following order:
/// 1. As a real file in the device directory ("in_distance_raw",
///    "in_voltage4_raw"). This is used by plain IIO devices such as srf04.
/// 2. As a logical name from the DT "channels" property of an IIO mux
///    ("battery", "m1"). The name is mapped to its index in the DT string
///    array and then to "in_voltage<index>_raw".
///
/// For an absolute @p label the device directory is used as-is and the
/// resulting file path is returned without an existence check.
///
/// @param label - DT label of the IIO device, or an absolute path to its
///        directory.
/// @param channel - sysfs channel file name or DT mux channel name.
/// @return full path to the channel file, or an empty string when the device
///         or channel is not found.
QString findIioChannelFileByLabel(const QString &label, const QString &channel);

}
