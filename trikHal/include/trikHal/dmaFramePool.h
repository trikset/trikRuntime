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

#include <memory>
#include <mutex>
#include <vector>

#include "trikHal/dmaHeapInterface.h"
#include "trikHal/framePool.h"
#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// FramePool backed by a DmaHeapInterface.
///
/// Buffers are allocated once for a fixed byte capacity and can be re-typed by
/// acquire() as any FrameFormat that fits. The pool keeps one reference to
/// every buffer, so an acquired frame stays valid even if the pool itself is
/// destroyed while a consumer still holds it.
class TRIKHAL_EXPORT DmaFramePool : public FramePool
{
public:
	/// Creates a pool with @p count buffers of @p bufferCapacity bytes each.
	static std::shared_ptr<DmaFramePool> create(DmaHeapInterface &heap, size_t bufferCapacity, int count);

	~DmaFramePool() override;

	FrameBufferPtr acquire(const FrameFormat &format) override;
	size_t bufferCapacity() const override { return mBufferCapacity; }
	int capacity() const override { return static_cast<int>(mSlots.size()); }

private:
	struct Slot;

	DmaFramePool(size_t bufferCapacity);
	bool init(DmaHeapInterface &heap, int count);

	size_t mBufferCapacity;
	mutable std::mutex mMutex;
	std::vector<std::unique_ptr<Slot>> mSlots;
};

} // namespace trikHal
