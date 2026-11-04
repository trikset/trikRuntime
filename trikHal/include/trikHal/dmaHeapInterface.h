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

#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// A CPU-accessible dma-buf allocated from a heap.
///
/// The buffer is contiguous on the target (CMA heap), so it can be imported by
/// DMA-capable hardware (RGA, NPU, ...). The owner must call syncStart()/
/// syncEnd() around CPU access when the heap is cache-coherent.
class TRIKHAL_EXPORT DmaBufferInterface
{
public:
	virtual ~DmaBufferInterface() = default;

	/// dma-buf file descriptor for hardware import.
	virtual int fd() const = 0;

	/// CPU mapping of the buffer, valid for the object's lifetime.
	virtual void *data() const = 0;

	virtual size_t size() const = 0;

	/// Begin CPU access. @p write is true when the CPU is going to write.
	virtual bool syncStart(bool write) = 0;

	/// End CPU access. @p write is true when the CPU has written the buffer.
	virtual bool syncEnd(bool write) = 0;
};

/// Allocates dma-bufs from a system heap.
///
/// The target implementation uses the low CMA heap (/dev/dma_heap/reserved) so
/// the resulting buffers are below the 4 GiB limit of the RGA. The caller owns
/// every buffer returned by allocate().
class TRIKHAL_EXPORT DmaHeapInterface
{
public:
	virtual ~DmaHeapInterface() = default;

	/// Returns a new buffer of @p size bytes, or nullptr when the heap is not
	/// available or the allocation failed (the reason is logged).
	virtual DmaBufferInterface *allocate(size_t size) = 0;
};

} // namespace trikHal
