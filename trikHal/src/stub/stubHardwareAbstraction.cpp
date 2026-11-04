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

#include "stubHardwareAbstraction.h"

#include "stubPeripheryI2c.h"
#include "stubMspUsb.h"
#include "stubUsbDevice.h"
#include "stubSystemConsole.h"
#include "stubEventFile.h"
#include "stubInputDeviceFile.h"
#include "stubOutputDeviceFile.h"
#include "stubFifo.h"
#include "stubIIOFile.h"
#include "stubImuFile.h"
#include "stubVideoDeviceFile.h"
#include "stubFbOutput.h"
#include "stubRga.h"
#include "stubDmaHeap.h"
#include "QsLog.h"

using namespace trikHal;
using namespace trikHal::stub;

StubHardwareAbstraction::StubHardwareAbstraction()
	: mI2c(new StubPeripheryI2c())
	, mSystemConsole(new StubSystemConsole())
{
}

StubHardwareAbstraction::~StubHardwareAbstraction() = default;

PeripheryI2cInterface &StubHardwareAbstraction::peripheryI2c()
{
	return *mI2c.data();
}

PeripheryI2cInterface *StubHardwareAbstraction::createPeripheryI2c()
{
	return new StubPeripheryI2c();
}

PeripheryI2cInterface *StubHardwareAbstraction::createCommonI2c(uint8_t regSize)
{
	Q_UNUSED(regSize)
	return new StubPeripheryI2c();
}

UsbDeviceInterface *StubHardwareAbstraction::createUsbDevice()
{
#ifdef TRIK_PERIPHERY_NRF
	return new StubUsbDevice();
#else
	return new StubMspUsb();
#endif
}

SystemConsoleInterface &StubHardwareAbstraction::systemConsole()
{
	return *mSystemConsole.data();
}

EventFileInterface *StubHardwareAbstraction::createEventFile(const QString &fileName) const
{
	return new StubEventFile(fileName);
}

FifoInterface *StubHardwareAbstraction::createFifo(const QString &fileName) const
{
	return new StubFifo(fileName);
}

IIOFileInterface *StubHardwareAbstraction::createIIOFile(const QString &fileName, const QString &scanType) const
{
	return new StubIIOFile(fileName, scanType);
}

ImuFileInterface *StubHardwareAbstraction::createImuFile(const QString &fileName) const
{
	return new StubImuFile(fileName);
}

InputDeviceFileInterface *StubHardwareAbstraction::createInputDeviceFile(const QString &fileName) const
{
	return new StubInputDeviceFile(fileName);
}

OutputDeviceFileInterface *StubHardwareAbstraction::createOutputDeviceFile(const QString &fileName) const
{
	return new StubOutputDeviceFile(fileName);
}

VideoDeviceFileInterface *StubHardwareAbstraction::createVideoDeviceFile( // NOLINT(google-default-arguments)
	const QString &devicePath, uint32_t width, uint32_t height, uint32_t fourcc, bool isWebcam) const
{
	Q_UNUSED(width);
	Q_UNUSED(height);
	Q_UNUSED(fourcc);
	Q_UNUSED(isWebcam);
	return new StubVideoDeviceFile(devicePath);
}

bool StubHardwareAbstraction::initVideoSensor(const QString &deviceFile, int i2cBus, int i2cAddress,
	int gpioNumber) const
{
	Q_UNUSED(deviceFile);
	Q_UNUSED(i2cBus);
	Q_UNUSED(i2cAddress);
	Q_UNUSED(gpioNumber);
	return true;
}

FbOutputInterface *StubHardwareAbstraction::createFbOutput() const
{
	return new stub::StubFbOutput();
}

RgaInterface *StubHardwareAbstraction::createRga() const
{
	return new stub::StubRga();
}

DmaHeapInterface *StubHardwareAbstraction::createDmaHeap() const
{
	return new stub::StubDmaHeap();
}
