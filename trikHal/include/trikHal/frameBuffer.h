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

#include <atomic>
#include <cstdint>
#include <memory>
#include <utility>

#include "trikHal/dmaHeapInterface.h"
#include "trikHal/frameFormat.h"
#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

namespace detail {

/// Process-wide unique id source for FrameBuffer objects. Inline function
/// guarantees a single counter across translation units.
inline uint64_t nextFrameBufferId()
{
	static std::atomic<uint64_t> next {1};
	return next.fetch_add(1, std::memory_order_relaxed);
}

} // namespace detail

/// One dma-buf plus the format needed to import it into a hardware backend.
///
/// This is the same dma-buf as the one produced by DmaHeapInterface, wrapped
/// with metadata and ownership; it is not a second allocator. Hardware
/// producers fill it (RGA, CPU, NPU) and consumers import it (GPU preview,
/// another RGA pass, ...). Lifetime is handled by FrameBufferPtr; when the
/// last reference is dropped a pool may reuse the buffer.
///
/// A pool may reuse the same object for another FrameFormat when the buffer is
/// not currently acquired; setFormat() is only called in that window.
class TRIKHAL_EXPORT FrameBuffer
{
public:
	FrameBuffer(std::unique_ptr<DmaBufferInterface> dma, const FrameFormat &format)
		: mDma(std::move(dma))
		, mFormat(format)
		, mId(detail::nextFrameBufferId())
	{
	}

	FrameBuffer(const FrameBuffer &) = delete;
	FrameBuffer &operator=(const FrameBuffer &) = delete;

	/// Stable id of this buffer allocation. Unlike fd(), it is unique for the
	/// lifetime of the process and never reused, so consumers can cache derived
	/// GPU resources by it.
	uint64_t id() const { return mId; }

	const FrameFormat &format() const { return mFormat; }

	/// Re-types the buffer. The pool calls this only while the buffer is free;
	/// no consumer may hold the FrameBuffer at that moment.
	void setFormat(const FrameFormat &format) { mFormat = format; }

	int fd() const { return mDma->fd(); }
	void *data() const { return mDma->data(); }
	size_t size() const { return mDma->size(); }
	bool syncStart(bool write) { return mDma->syncStart(write); }
	bool syncEnd(bool write) { return mDma->syncEnd(write); }

private:
	std::unique_ptr<DmaBufferInterface> mDma;
	FrameFormat mFormat;
	uint64_t mId;
};

using FrameBufferPtr = std::shared_ptr<FrameBuffer>;

} // namespace trikHal
