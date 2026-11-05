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

#include "trikHal/frameBuffer.h"
#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// Fixed-count source of reusable dma-buf frames.
///
/// The pool is parameterised by buffer capacity, not by one fixed frame
/// format: each acquire() re-types a free buffer as the requested FrameFormat.
/// This lets idle pools be handed to clients with different (but fitting)
/// formats without touching the dma-heap.
///
/// acquire() never blocks: it returns nullptr when every buffer is still held
/// by a consumer or when the requested format does not fit, so callers can drop
/// the frame (latest-wins) instead of stalling the capture pipeline.
class TRIKHAL_EXPORT FramePool
{
public:
	virtual ~FramePool() = default;

	/// Acquires a free buffer and types it as @p format.
	/// Returns nullptr if no buffer is free or @p format.byteSize() exceeds
	/// bufferCapacity().
	virtual FrameBufferPtr acquire(const FrameFormat &format) = 0;

	/// Allocated bytes per buffer. Every acquired FrameFormat must fit here.
	virtual size_t bufferCapacity() const = 0;

	/// Number of buffers in the pool.
	virtual int capacity() const = 0;
};

} // namespace trikHal
