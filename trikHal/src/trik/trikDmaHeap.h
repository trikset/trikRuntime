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

#include <trikHal/dmaHeapInterface.h>

namespace trikHal {
namespace trik {

/// DmaHeap backed by the low CMA heap (/dev/dma_heap/reserved), with
/// /dev/dma_heap/cma as a fallback. Buffers are contiguous and below 4 GiB, so
/// the RGA can import them.
class TrikDmaHeap : public DmaHeapInterface
{
public:
	TrikDmaHeap();
	~TrikDmaHeap() override;

	TrikDmaHeap(const TrikDmaHeap &) = delete;
	TrikDmaHeap &operator=(const TrikDmaHeap &) = delete;

	DmaBufferInterface *allocate(size_t size) override;

private:
	int mHeapFd = -1;
};

} // namespace trik
} // namespace trikHal
