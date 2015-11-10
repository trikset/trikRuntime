/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "led.h"

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include <QsLog.h>

using namespace trikControl;

Led::Led(const trikKernel::Configurer &configurer, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mRedDeviceFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByDevice("led", "red")))
	, mGreenDeviceFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByDevice("led", "green")))
	, mState("Led")
{
	if (!mRedDeviceFile->open()) {
		mState.fail();
	}

	if (!mGreenDeviceFile->open()) {
		mState.fail();
	}

	mState.ready();
}

Led::~Led()
{
	mRedDeviceFile->close();
	mGreenDeviceFile->close();
}

Led::Status Led::status() const
{
	return mState.status();
}

void Led::red()
{
	if (mState.isReady()) {
		mRedDeviceFile->write("1");
		mGreenDeviceFile->write("0");
	}
}

void Led::green()
{
	if (mState.isReady()) {
		mRedDeviceFile->write("0");
		mGreenDeviceFile->write("1");
	}
}

void Led::orange()
{
	if (mState.isReady()) {
		mRedDeviceFile->write("1");
		mGreenDeviceFile->write("1");
	}
}

void Led::off()
{
	if (mState.isReady()) {
		mRedDeviceFile->write("0");
		mGreenDeviceFile->write("0");
	}
}
