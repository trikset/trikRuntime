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

#include "trikDmaBuffer.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <linux/dma-buf.h>

using namespace trikHal;
using namespace trikHal::trik;

TrikDmaBuffer::TrikDmaBuffer(int fd, void *data, size_t size)
	: mFd(fd)
	, mData(data)
	, mSize(size)
{
}

TrikDmaBuffer::~TrikDmaBuffer()
{
	if (mData && mData != MAP_FAILED) {
		munmap(mData, mSize);
	}
	if (mFd >= 0) {
		close(mFd);
	}
}

int TrikDmaBuffer::fd() const
{
	return mFd;
}

void *TrikDmaBuffer::data() const
{
	return mData;
}

size_t TrikDmaBuffer::size() const
{
	return mSize;
}

bool TrikDmaBuffer::syncStart(bool write)
{
	return sync(true, write);
}

bool TrikDmaBuffer::syncEnd(bool write)
{
	return sync(false, write);
}

bool TrikDmaBuffer::sync(bool start, bool write) const
{
	auto sync = dma_buf_sync {};
	sync.flags = (start ? DMA_BUF_SYNC_START : DMA_BUF_SYNC_END) | (write ? DMA_BUF_SYNC_WRITE : DMA_BUF_SYNC_READ);
	return ioctl(mFd, DMA_BUF_IOCTL_SYNC, &sync) == 0;
}
