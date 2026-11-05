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

#include <trikHal/dmaFramePool.h>

#include <memory>
#include <utility>

#include <QsLog.h>

using namespace trikHal;

struct DmaFramePool::Slot {
	std::shared_ptr<FrameBuffer> frame;
};

std::shared_ptr<DmaFramePool> DmaFramePool::create(DmaHeapInterface &heap, size_t bufferCapacity, int count)
{
	if (count <= 0 || bufferCapacity == 0) {
		QLOG_ERROR() << "DmaFramePool: invalid pool parameters (count" << count << "capacity" << bufferCapacity
			     << ")";
		return nullptr;
	}

	std::shared_ptr<DmaFramePool> pool(new DmaFramePool(bufferCapacity));
	if (!pool->init(heap, count)) {
		return nullptr;
	}

	return pool;
}

DmaFramePool::DmaFramePool(size_t bufferCapacity)
	: mBufferCapacity(bufferCapacity)
{
}

DmaFramePool::~DmaFramePool() = default;

bool DmaFramePool::init(DmaHeapInterface &heap, int count)
{
	mSlots.reserve(static_cast<size_t>(count));

	for (int i = 0; i < count; ++i) {
		std::unique_ptr<DmaBufferInterface> dma(heap.allocate(mBufferCapacity));
		if (!dma) {
			QLOG_ERROR() << "DmaFramePool: failed to allocate frame" << i << "of" << count;
			mSlots.clear();
			return false;
		}

		auto slot = std::make_unique<Slot>();
		// The format is set per acquire(); the empty value here is never
		// observed by a consumer because acquire() types the buffer first.
		slot->frame = std::make_shared<FrameBuffer>(std::move(dma), FrameFormat {});
		mSlots.push_back(std::move(slot));
	}

	return true;
}

FrameBufferPtr DmaFramePool::acquire(const FrameFormat &format)
{
	if (format.width == 0 || format.height == 0 || format.stride == 0 || format.byteSize() > mBufferCapacity) {
		QLOG_WARN() << "DmaFramePool: frame" << format.width << "x" << format.height << "stride"
			    << format.stride << "does not fit into" << mBufferCapacity << "bytes";
		return nullptr;
	}

	std::lock_guard<std::mutex> lock(mMutex);

	for (auto &slot : mSlots) {
		// The pool itself always holds one reference, so use_count() == 1 means
		// no consumer owns this buffer right now.
		if (slot->frame.use_count() == 1) {
			slot->frame->setFormat(format);
			return slot->frame;
		}
	}

	return nullptr;
}
