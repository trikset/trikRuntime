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

#include "physicalMemoryMapper.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <QsLog.h>

namespace trikHal {

namespace {
constexpr int PAGE_SIZE = 4096;
}

MappedMemory::~MappedMemory()
{
	reset();
}

void MappedMemory::reset()
{
	if (mBase) {
		::munmap(mBase, mLength);
	}
	mBase = nullptr;
	mLength = 0;
	mData = nullptr;
}

MappedMemory::MappedMemory(MappedMemory &&other) noexcept
	: mBase(other.mBase)
	, mLength(other.mLength)
	, mData(other.mData)
{
	other.mBase = nullptr;
	other.mLength = 0;
	other.mData = nullptr;
}

MappedMemory &MappedMemory::operator=(MappedMemory &&other) noexcept
{
	if (this != &other) {
		reset();
		mBase = other.mBase;
		mLength = other.mLength;
		mData = other.mData;
		other.mBase = nullptr;
		other.mLength = 0;
		other.mData = nullptr;
	}
	return *this;
}

MappedMemory mapPhysicalMemory(uintptr_t physAddr, size_t length)
{
	const auto pageBase = physAddr / PAGE_SIZE * PAGE_SIZE;
	const auto pageOffset = physAddr - pageBase;

	const int memfd = ::open("/dev/mem", O_RDWR | O_SYNC);
	if (memfd < 0) {
		QLOG_ERROR() << "mapPhysicalMemory: open /dev/mem failed:" << strerror(errno);
		return {};
	}

	const size_t mapLen = pageOffset + length;
	auto *mapped = ::mmap(nullptr, mapLen, PROT_READ | PROT_WRITE, MAP_SHARED,
	                      memfd, static_cast<off_t>(pageBase));
	if (mapped == MAP_FAILED) {
		QLOG_ERROR() << "mapPhysicalMemory: mmap /dev/mem failed:" << strerror(errno);
		::close(memfd);
		return {};
	}

	MappedMemory result;
	result.mBase = mapped;
	result.mLength = mapLen;
	result.mData = static_cast<uint8_t *>(mapped) + pageOffset;
	return result;
}

}
