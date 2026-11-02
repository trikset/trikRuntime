/* Copyright 2013 - 2015 Roman Kurbatov, Yurii Litvinov and CyberTech Labs Ltd.
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

#include "pwmCapture.h"

#include <algorithm>

#include <QtCore/QTextStream>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include "configurerHelper.h"

#include <QsLog.h>

using namespace trikControl;

/// The current platform has PWM blocks whose channels can work as inputs using
/// the same sysfs files as for output. The kernel already guards switching to
/// capture mode: if period or duty is not zero, the port simply will not be
/// switched into this mode. It is therefore the responsibility of devices that
/// use PWM (servoMotor, powerMotor) to reset period and duty to zero in their
/// destructors/stop()/powerOff(); PwmCapture does not reset them itself.
PwmCapture::PwmCapture(const QString &port, const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mCaptureFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "captureFile")))
	, mState("PWM Capture on " + port)
{
	if (!mCaptureFile->open()) {
		mState.fail();
		return;
	}

	mMinValue = ConfigurerHelper::configureLong(configurer, mState, port, "minValue");
	mMaxValue = ConfigurerHelper::configureLong(configurer, mState, port, "maxValue");
	mMinValueScaled = ConfigurerHelper::configureLong(configurer, mState, port, "minValueScaled");
	mMaxValueScaled = ConfigurerHelper::configureLong(configurer, mState, port, "maxValueScaled");

	if (mMinValue == mMaxValue) {
		QLOG_ERROR() << "PWM Capture configuration error: minValue = maxValue!";
		mState.fail();
		mK = 0;
		mB = 0;
	} else {
		mK = static_cast<qreal>(mMaxValueScaled - mMinValueScaled) / static_cast<qreal>(mMaxValue - mMinValue);
		mB = static_cast<qreal>(mMinValueScaled) - mK * static_cast<qreal>(mMinValue);
	}

	mState.ready();
}

PwmCapture::~PwmCapture() = default;

PwmCapture::Status PwmCapture::status() const
{
	return mState.status();
}

QVector<int> PwmCapture::frequency()
{
	if (!mState.isReady()) {
		return {};
	}

	mCaptureFile->reset();
	auto period = 0;
	auto cycle = 0;
	mCaptureFile->stream() >> period >> cycle;
	return {period, cycle};
}

long PwmCapture::duty()
{
	if (!mState.isReady()) {
		return {};
	}

	auto data = dutyRaw();

	auto result = std::min(mMaxValue, std::max(mMinValue, data));
	result = static_cast<long>(mK * static_cast<qreal>(result) + mB);

	return result;
}

long PwmCapture::dutyRaw()
{
	if (!mState.isReady()) {
		return {};
	}

	mCaptureFile->reset();
	auto period = 0;
	auto cycle = 0;
	mCaptureFile->stream() >> period >> cycle;
	return cycle;
}

long PwmCapture::minValue() const
{
	return mMinValueScaled;
}

long PwmCapture::maxValue() const
{
	return mMaxValueScaled;
}
