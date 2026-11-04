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

#include <trikHal/dmaHeapInterface.h>

namespace trikHal {
namespace trik {

/// DmaBufferInterface backed by a mapped dma-buf. Owns the file descriptor and
/// the CPU mapping, releasing both on destruction.
class TrikDmaBuffer : public DmaBufferInterface
{
public:
	/// Takes ownership of @p fd and @p data. @p data must be a mapping of
	/// @p size bytes created from @p fd.
	TrikDmaBuffer(int fd, void *data, size_t size);
	~TrikDmaBuffer() override;

	TrikDmaBuffer(const TrikDmaBuffer &) = delete;
	TrikDmaBuffer &operator=(const TrikDmaBuffer &) = delete;

	int fd() const override;
	void *data() const override;
	size_t size() const override;

	bool syncStart(bool write) override;
	bool syncEnd(bool write) override;

private:
	bool sync(bool start, bool write) const;

	int mFd;
	void *mData;
	size_t mSize;
};

} // namespace trik
} // namespace trikHal
