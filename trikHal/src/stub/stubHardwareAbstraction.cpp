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

#include "stubMspI2c.h"
#include "stubMspUsb.h"
#include "stubSystemConsole.h"
#include "stubEventFile.h"
#include "stubInputDeviceFile.h"
#include "stubOutputDeviceFile.h"
#include "stubFifo.h"
#include "stubIIOFile.h"

#include "QsLog.h"

using namespace trikHal;
using namespace trikHal::stub;

StubHardwareAbstraction::StubHardwareAbstraction()
	: mMspI2cBus(new StubMspI2C())
	, mMspUsbBus(new StubMspUsb())
	, mSystemConsole(new StubSystemConsole())
{
}

StubHardwareAbstraction::~StubHardwareAbstraction()
{
}

MspI2cInterface &StubHardwareAbstraction::mspI2c()
{
	return *mMspI2cBus.data();
}

MspUsbInterface &StubHardwareAbstraction::mspUsb()
{
	return *mMspUsbBus.data();
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

IIOFileInterface *StubHardwareAbstraction::createIIOFile(const QString &fileName) const
{
	return new StubIIOFile(fileName);
}

InputDeviceFileInterface *StubHardwareAbstraction::createInputDeviceFile(const QString &fileName) const
{
	return new StubInputDeviceFile(fileName);
}

OutputDeviceFileInterface *StubHardwareAbstraction::createOutputDeviceFile(const QString &fileName) const
{
	return new StubOutputDeviceFile(fileName);
}

QVector<uint8_t> StubHardwareAbstraction::captureV4l2StillImage(const QString &port, const QDir &pathToPic) const
{
	Q_UNUSED(pathToPic);
	QLOG_INFO() << "Call stub HAL captureV4l2StillImage on port " + port;
	return QVector<uint8_t>();
}
