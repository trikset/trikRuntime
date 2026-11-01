/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include "battery.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/inputDeviceFileInterface.h>

#include <QsLog.h>

using namespace trikControl;

Battery::Battery(const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Battery")
{
	auto deviceFile = QString();
	try {
		deviceFile = configurer.attributeByDevice("battery", "deviceFile");
	} catch (trikKernel::MalformedConfigException &) {
		QLOG_ERROR() << "Battery: deviceFile is not configured";
		mState.fail();
		return;
	}

	mVoltageFile.reset(hardwareAbstraction.createInputDeviceFile(deviceFile));
	if (!mVoltageFile->open()) {
		QLOG_ERROR() << "Battery: failed to open" << deviceFile;
		mState.fail();
		return;
	}

	const auto scaleFile = QFileInfo(deviceFile).absolutePath() + QStringLiteral("/in_voltage_scale");
	QScopedPointer<trikHal::InputDeviceFileInterface> scaleFileHandle(
		hardwareAbstraction.createInputDeviceFile(scaleFile));
	if (scaleFileHandle->open()) {
		auto scale = 0.0;
		scaleFileHandle->stream() >> scale;
		scaleFileHandle->close();
		if (scale > 0.0) {
			mScaleMv = scale;
		}
	} else {
		QLOG_WARN() << "Battery: failed to open" << scaleFile << ", using default scale";
	}

	try {
		mDivider = configurer.attributeByDevice("battery", "divider").toDouble();
		if (mDivider <= 0.0) {
			QLOG_ERROR() << "Battery: divider must be positive";
			mState.fail();
		}
	} catch (trikKernel::MalformedConfigException &) {
		QLOG_ERROR() << "Battery: divider is not configured";
		mState.fail();
	}

	if (mState.isFailed()) {
		return;
	}

	mState.ready();
}

Battery::~Battery()
{
	if (mVoltageFile) {
		mVoltageFile->close();
	}
}

float Battery::readVoltage()
{
	return static_cast<float>(readRawDataVoltage() * mScaleMv * mDivider / 1000.0);
}

float Battery::readRawDataVoltage()
{
	if (!mState.isReady()) {
		return 0;
	}

	mVoltageFile->reset();

	auto raw = 0;
	mVoltageFile->stream() >> raw;

	return static_cast<float>(raw);
}

Battery::Status Battery::status() const
{
	return mState.status();
}
