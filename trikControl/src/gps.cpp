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

#include "gps.h"

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <QsLog.h>

#include "configurerHelper.h"

using namespace trikControl;

Gps::Gps(const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mDeviceFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByDevice("gps", "deviceFile")))
	, mState("GPS")
{
	mState.ready();
}

Gps::~Gps()
{
}

QVector<int> Gps::read()
{
	if (!mState.isReady()) {
		return {};
	}

	if (!mDeviceFile->open()) {
		mState.fail();
		return {};
	}

	/// Stub implementation, does nothing.

	QString buffer;
	mDeviceFile->stream() >> buffer;

	mDeviceFile->close();

	return {};
}

Gps::Status Gps::status() const
{
	return mState.status();
}
