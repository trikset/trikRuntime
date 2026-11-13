/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "trikHardwareAbstraction.h"

#include "trikI2c.h"
#include "trikMspUsb.h"
#include "trikUsbDevice.h"
#include "trikSystemConsole.h"
#include "trikEventFile.h"
#include "trikInputDeviceFile.h"
#include "trikOutputDeviceFile.h"
#include "trikFifo.h"
#include "trikIIOFile.h"
#include "trikVideoDevice.h"
#include "QsLog.h"
#include "commonI2c.h"
#include "trikRga.h"
#include "trikFbOutput.h"
#include "trikDmaHeap.h"
#include <trikHal/framePoolAllocator.h>

#include <memory>

#include <QtCore/QScopedPointer>
#include <sys/stat.h>

using namespace trikHal;
using namespace trikHal::trik;

namespace {

/// Number of V4L2 buffers for a capture port.
constexpr uint32_t defaultVideoBufferCount = 3;

} // namespace

TrikHardwareAbstraction::TrikHardwareAbstraction()
	: mI2c(new TrikI2c())
	, mSystemConsole(new TrikSystemConsole())
	, mFramePoolAllocator(
		  std::make_shared<FramePoolAllocator>(std::unique_ptr<DmaHeapInterface>(new TrikDmaHeap())))
{
}

TrikHardwareAbstraction::~TrikHardwareAbstraction() = default;

PeripheryI2cInterface &TrikHardwareAbstraction::peripheryI2c()
{
	return *mI2c.data();
}

PeripheryI2cInterface *TrikHardwareAbstraction::createPeripheryI2c()
{
	return new TrikI2c();
}

PeripheryI2cInterface *TrikHardwareAbstraction::createCommonI2c(uint8_t regSize)
{
	return new CommonI2c(regSize);
}

UsbDeviceInterface *TrikHardwareAbstraction::createUsbDevice()
{
#ifdef TRIK_PERIPHERY_NRF
#	ifndef TRIK_NOT_BRICK
	struct stat st {};
	if (::stat("/dev/nrf0", &st) == 0 && S_ISCHR(st.st_mode)) {
		QLOG_INFO() << "TrikHardwareAbstraction: using kernel nrf driver";
		return new TrikUsbDevice();
	}
	QLOG_ERROR() << "TrikHardwareAbstraction: /dev/nrf0 not found - kernel driver not loaded";
#	endif
	return nullptr;
#else
	return new TrikMspUsb();
#endif
}

SystemConsoleInterface &TrikHardwareAbstraction::systemConsole()
{
	return *mSystemConsole.data();
}

EventFileInterface *TrikHardwareAbstraction::createEventFile(const QString &fileName) const
{
	return new TrikEventFile(fileName);
}

FifoInterface *TrikHardwareAbstraction::createFifo(const QString &fileName) const
{
	return new TrikFifo(fileName);
}

IIOFileInterface *TrikHardwareAbstraction::createIIOFile(const QString &fileName) const
{
	return new TrikIIOFile(fileName);
}

InputDeviceFileInterface *TrikHardwareAbstraction::createInputDeviceFile(const QString &fileName) const
{
	return new TrikInputDeviceFile(fileName);
}

OutputDeviceFileInterface *TrikHardwareAbstraction::createOutputDeviceFile(const QString &fileName) const
{
	return new TrikOutputDeviceFile(fileName);
}

VideoDeviceFileInterface *TrikHardwareAbstraction::createVideoDeviceFile( // NOLINT(google-default-arguments)
	const QString &devicePath, uint32_t width, uint32_t height, uint32_t fourcc, bool isWebcam) const
{
	return new TrikVideoDevice(devicePath, width, height, fourcc, defaultVideoBufferCount, isWebcam);
}

FbOutputInterface *TrikHardwareAbstraction::createFbOutput() const
{
	return new trik::TrikFbOutput();
}

RgaInterface *TrikHardwareAbstraction::createRga() const
{
	return new trik::TrikRga();
}

DmaHeapInterface *TrikHardwareAbstraction::createDmaHeap() const
{
	return new trik::TrikDmaHeap();
}

FrameBufferPtr TrikHardwareAbstraction::acquireFrameBuffer(const FrameFormat &format, int count)
{
	if (!mFramePoolAllocator) {
		return nullptr;
	}

	// The pool lease is released immediately: the allocator keeps the pool in
	// its idle cache and the returned FrameBufferPtr keeps only the buffer it
	// actually uses. A later acquire() reuses the same idle pool.
	auto pool = mFramePoolAllocator->acquire(format, count);
	if (!pool) {
		return nullptr;
	}

	return pool->acquire(format);
}
