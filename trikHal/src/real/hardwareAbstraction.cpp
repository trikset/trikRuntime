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

#include "hardwareAbstraction.h"

#include "i2c.h"
#include "systemConsole.h"
#include "eventFile.h"
#include "fifo.h"

using namespace trikHal;

HardwareAbstraction::HardwareAbstraction()
	: mI2c(new I2C())
	, mSystemConsole(new SystemConsole())
{
}

HardwareAbstraction::~HardwareAbstraction()
{
}

I2CInterface &HardwareAbstraction::i2c()
{
	return *mI2c.data();
}

SystemConsoleInterface &HardwareAbstraction::systemConsole()
{
	return *mSystemConsole.data();
}

EventFileInterface *HardwareAbstraction::createEventFile() const
{
	return new EventFile();
}

FifoInterface *HardwareAbstraction::createFifo() const
{
	return new Fifo();
}
