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

#include <trikHal/framePoolAllocator.h>

#include <utility>

using namespace trikHal;

namespace {

constexpr size_t allocationGranularity = 4096;
constexpr size_t idleBudgetBytes = 8 * 1024 * 1024;

size_t alignUp(size_t value)
{
	return (value + allocationGranularity - 1) & ~(allocationGranularity - 1);
}

} // namespace

FramePoolAllocator::FramePoolAllocator(std::unique_ptr<DmaHeapInterface> heap)
	: mHeap(std::move(heap))
{
}

FramePoolAllocator::~FramePoolAllocator() = default;

std::shared_ptr<FramePool> FramePoolAllocator::acquire(const FrameFormat &format, int count)
{
	if (!mHeap || count <= 0 || format.width == 0 || format.height == 0 || format.byteSize() == 0) {
		return nullptr;
	}

	const size_t neededCapacity = alignUp(format.byteSize());

	std::shared_ptr<DmaFramePool> pool;
	{
		std::lock_guard<std::mutex> lock(mMutex);
		pool = takeIdleLocked(neededCapacity, count);
		if (!pool) {
			pool = DmaFramePool::create(*mHeap, neededCapacity, count);
		}
	}

	if (!pool) {
		return nullptr;
	}

	// The pool itself is owned by the deleter until the client releases the
	// lease. On release it is returned to the idle cache, or destroyed when the
	// allocator is already gone.
	auto weak = std::weak_ptr<FramePoolAllocator>(shared_from_this());
	return {pool.get(), [weak, pool](FramePool *) {
		if (auto allocator = weak.lock()) {
			allocator->recycle(pool);
		}
	}};
}

void FramePoolAllocator::recycle(const std::shared_ptr<DmaFramePool> &pool)
{
	if (!pool) {
		return;
	}

	std::lock_guard<std::mutex> lock(mMutex);
	mIdle.push_back(pool);
	mIdleBytes += poolBytes(pool);
	evictIdleLocked();
}

std::shared_ptr<DmaFramePool> FramePoolAllocator::takeIdleLocked(size_t neededCapacity, int count)
{
	auto bestIt = mIdle.end();
	size_t bestCapacity = 0;

	for (auto it = mIdle.begin(); it != mIdle.end(); ++it) {
		const auto &pool = *it;
		if (pool->capacity() != count || pool->bufferCapacity() < neededCapacity) {
			continue;
		}

		if (bestIt == mIdle.end() || pool->bufferCapacity() < bestCapacity) {
			bestIt = it;
			bestCapacity = pool->bufferCapacity();
		}
	}

	if (bestIt == mIdle.end()) {
		return nullptr;
	}

	auto pool = *bestIt;
	mIdle.erase(bestIt);
	mIdleBytes -= poolBytes(pool);
	return pool;
}

void FramePoolAllocator::evictIdleLocked()
{
	// mIdle is in LRU order: front is the oldest released pool.
	while (mIdleBytes > idleBudgetBytes && !mIdle.empty()) {
		auto victim = mIdle.front();
		mIdle.erase(mIdle.begin());
		mIdleBytes -= poolBytes(victim);
	}
}

size_t FramePoolAllocator::poolBytes(const std::shared_ptr<DmaFramePool> &pool)
{
	return static_cast<size_t>(pool->capacity()) * pool->bufferCapacity();
}
