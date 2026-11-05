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
#include <memory>
#include <mutex>
#include <vector>

#include "trikHal/dmaFramePool.h"
#include "trikHal/dmaHeapInterface.h"
#include "trikHal/framePool.h"
#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// Creates format-agnostic FramePools and recycles idle ones by buffer capacity.
///
/// A pool is parameterised by capacity and buffer count, not by a frame format.
/// When a client releases its lease, the pool is kept in an idle cache and can
/// be handed to another client whose FrameFormat fits into the same capacity.
///
/// The lease is advisory: a client that releases it while still holding some
/// buffers makes the pool available for reuse, but DmaFramePool's per-slot
/// refcounts prevent those buffers from being overwritten until they are
/// released by their consumers.
///
/// The allocator must be owned by a std::shared_ptr (acquire() uses
/// shared_from_this() to build the recycling deleter).
class TRIKHAL_EXPORT FramePoolAllocator : public std::enable_shared_from_this<FramePoolAllocator>
{
public:
	explicit FramePoolAllocator(std::unique_ptr<DmaHeapInterface> heap);
	~FramePoolAllocator();

	FramePoolAllocator(const FramePoolAllocator &) = delete;
	FramePoolAllocator &operator=(const FramePoolAllocator &) = delete;

	/// Returns a pool of @p count buffers that can hold @p format.
	///
	/// The returned shared_ptr owns a lease: when the last reference is dropped,
	/// the pool goes back to the idle cache (or is destroyed when the cache is
	/// over budget). Returns nullptr when the heap cannot allocate.
	std::shared_ptr<FramePool> acquire(const FrameFormat &format, int count);


private:
	void recycle(const std::shared_ptr<DmaFramePool> &pool);
	std::shared_ptr<DmaFramePool> takeIdleLocked(size_t neededCapacity, int count);
	void evictIdleLocked();
	static size_t poolBytes(const std::shared_ptr<DmaFramePool> &pool);

	std::unique_ptr<DmaHeapInterface> mHeap;
	mutable std::mutex mMutex;
	std::vector<std::shared_ptr<DmaFramePool>> mIdle;
	size_t mIdleBytes = 0;
};

} // namespace trikHal
