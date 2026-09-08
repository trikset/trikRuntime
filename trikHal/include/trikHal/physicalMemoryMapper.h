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

#include <cstddef>
#include <cstdint>

#include "trikHalDeclSpec.h"

namespace trikHal {

/// RAII owner of a physical-memory mapping. munmaps the region on destruction,
/// so callers never have to pair mmap/munmap manually.
class TRIKHAL_EXPORT MappedMemory
{
public:
	MappedMemory() = default;
	MappedMemory(const MappedMemory &) = delete;
	MappedMemory &operator=(const MappedMemory &) = delete;
	MappedMemory(MappedMemory &&) noexcept;
	MappedMemory &operator=(MappedMemory &&) noexcept;
	~MappedMemory();

	/// Unmap the region (no-op when empty).
	void reset();

	/// Usable pointer (page-offset into the mapped region), or nullptr.
	uint8_t *data() const { return mData; }
	/// Page-aligned mmap base.
	void *base() const { return mBase; }
	/// Total mapped length in bytes.
	size_t length() const { return mLength; }

	/// Whether the mapping succeeded.
	explicit operator bool() const { return mData != nullptr; }

private:
	friend TRIKHAL_EXPORT MappedMemory mapPhysicalMemory(uintptr_t physAddr, size_t length);

	void *mBase = nullptr;
	size_t mLength = 0;
	uint8_t *mData = nullptr;
};

/// Map @p length bytes of physical memory starting at @p physAddr into the
/// process address space via /dev/mem (requires root). The mapping is page-
/// aligned internally; data() points at @p physAddr within it. Returns an empty
/// MappedMemory on failure (e.g. no /dev/mem).
TRIKHAL_EXPORT MappedMemory mapPhysicalMemory(uintptr_t physAddr, size_t length);

}
