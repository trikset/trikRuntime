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

#include "trikMspI2c.h"
#include "trikMspUsb.h"
#include "trikSystemConsole.h"
#include "trikEventFile.h"
#include "trikInputDeviceFile.h"
#include "trikOutputDeviceFile.h"
#include "trikFifo.h"

using namespace trikHal;
using namespace trikHal::trik;

TrikHardwareAbstraction::TrikHardwareAbstraction()
	: mI2c(new TrikMspI2c())
	, mUsb(new TrikMspUsb())
	, mSystemConsole(new TrikSystemConsole())
{
}

TrikHardwareAbstraction::~TrikHardwareAbstraction()
{
}

MspI2cInterface &TrikHardwareAbstraction::mspI2c()
{
	return *mI2c.data();
}

MspUsbInterface &TrikHardwareAbstraction::mspUsb()
{
	return *mUsb.data();
}

SystemConsoleInterface &TrikHardwareAbstraction::systemConsole()
{
	return *mSystemConsole.data();
}

EventFileInterface *TrikHardwareAbstraction::createEventFile(const QString &fileName, QThread &thread) const
{
	return new TrikEventFile(fileName, thread);
}

FifoInterface *TrikHardwareAbstraction::createFifo(const QString &fileName) const
{
	return new TrikFifo(fileName);
}

InputDeviceFileInterface *TrikHardwareAbstraction::createInputDeviceFile(const QString &fileName) const
{
	return new TrikInputDeviceFile(fileName);
}

OutputDeviceFileInterface *TrikHardwareAbstraction::createOutputDeviceFile(const QString &fileName) const
{
	return new TrikOutputDeviceFile(fileName);
}
