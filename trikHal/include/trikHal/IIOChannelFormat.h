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

/// Format of a single IIO scan channel, parsed from the sysfs `_type` string
/// (`<endian>:<sign><bits>/<storageBits>>>shift`, e.g. `le:s16/16>>0`).
struct IIOChannelFormat {
	/// Number of bits used to store the value.
	int storageBits = 0;

	/// Whether the stored value uses big-endian byte order.
	bool bigEndian = true;

	/// Whether the value is a signed integer.
	bool isSigned = true;

	/// Number of low bits that must be discarded from the raw value.
	int shift = 0;

	bool operator==(const IIOChannelFormat &other) const
	{
		return storageBits == other.storageBits && bigEndian == other.bigEndian && isSigned == other.isSigned
		       && shift == other.shift;
	}

	bool operator!=(const IIOChannelFormat &other) const
	{
		return !(*this == other);
	}
};

}
