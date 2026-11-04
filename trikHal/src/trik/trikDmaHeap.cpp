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

#include "trikDmaHeap.h"

#include "trikDmaBuffer.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <linux/dma-heap.h>

#include <QsLog.h>

using namespace trikHal;
using namespace trikHal::trik;

namespace {

constexpr size_t pageSize = 4096;

} // namespace

TrikDmaHeap::TrikDmaHeap()
{
	static const char *const paths[] = {
		"/dev/dma_heap/reserved",
		"/dev/dma_heap/cma",
	};

	for (const auto *path : paths) {
		mHeapFd = ::open(path, O_RDONLY | O_CLOEXEC);
		if (mHeapFd >= 0) {
			QLOG_INFO() << "TrikDmaHeap: using" << path;
			return;
		}
	}

	QLOG_WARN() << "TrikDmaHeap: no CMA dma-heap available, falling back to CPU paths";
}

TrikDmaHeap::~TrikDmaHeap()
{
	if (mHeapFd >= 0) {
		close(mHeapFd);
	}
}

DmaBufferInterface *TrikDmaHeap::allocate(size_t size)
{
	if (mHeapFd < 0 || size == 0) {
		return nullptr;
	}

	const auto alignedSize = (size + pageSize - 1) & ~(pageSize - 1);

	auto data = dma_heap_allocation_data {};
	data.len = alignedSize;
	data.fd_flags = O_RDWR | O_CLOEXEC;

	if (ioctl(mHeapFd, DMA_HEAP_IOCTL_ALLOC, &data) < 0) {
		QLOG_ERROR() << "TrikDmaHeap: DMA_HEAP_IOCTL_ALLOC failed:" << strerror(errno);
		return nullptr;
	}

	auto *mapped = mmap(nullptr, alignedSize, PROT_READ | PROT_WRITE, MAP_SHARED, static_cast<int>(data.fd), 0);
	if (mapped == MAP_FAILED) {
		QLOG_ERROR() << "TrikDmaHeap: mmap failed:" << strerror(errno);
		close(static_cast<int>(data.fd));
		return nullptr;
	}

	return new TrikDmaBuffer(static_cast<int>(data.fd), mapped, alignedSize);
}
