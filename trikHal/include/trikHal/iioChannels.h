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

namespace trikHal {

/// Channel type, mirroring the standard Linux IIO channel type vocabulary
/// (enum iio_chan_type). It is a shared vocabulary, not sensor-specific logic.
enum class IIOChanType {
	Accel,
	AnglVel,
	Magn,
	Temp,
	Timestamp,
	Unknown,
};

/// Channel axis/modifier, mirroring IIO_MOD_X/Y/Z. None is used for channels
/// without an axis (temperature, timestamp, ...).
enum class IIOAxis {
	X,
	Y,
	Z,
	None,
};

/// Identifies a channel by its IIO type and axis.
struct IIOChannelId {
	IIOChanType type = IIOChanType::Unknown;
	IIOAxis axis = IIOAxis::None;

	bool operator==(const IIOChannelId &other) const
	{
		return type == other.type && axis == other.axis;
	}

	bool operator!=(const IIOChannelId &other) const
	{
		return !(*this == other);
	}
};

}
