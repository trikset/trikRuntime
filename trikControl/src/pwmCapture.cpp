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
#include "port/portManager.h"

#include <QsLog.h>

using namespace trikControl;

/// PWM capture is exposed as a read-only "capture" attribute of an exported
/// channel. The channel must not be enabled: the rockchip driver returns -EBUSY
/// for capture while the PWM output is running.
PwmCapture::PwmCapture(const QString &port, const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction, PortManager &ports)
	: mPortLease(ports.acquire(port, PortMode::Pwm, QStringLiteral("pwmCapture")))
	, mState("PWM Capture on " + port)
{
	if (!mPortLease) {
		mState.fail();
		return;
	}

	const auto pwmDevice = mPortLease.device();
	if (pwmDevice.isEmpty()) {
		QLOG_ERROR() << "PWM Capture on" << port << "requires a PWM device on the port";
		failSetup();
		return;
	}

	mPwmChannel = PwmChannelLease(pwmDevice, hardwareAbstraction);
	if (!mPwmChannel) {
		QLOG_ERROR() << "PWM Capture on" << port << "cannot resolve PWM device" << pwmDevice;
		failSetup();
		return;
	}

	const auto channelDir = mPwmChannel.channelDir();
	mCaptureFile.reset(hardwareAbstraction.createInputDeviceFile(channelDir + QStringLiteral("/capture")));
	if (!mCaptureFile || !mCaptureFile->open()) {
		failSetup();
		return;
	}

	mMinValue = ConfigurerHelper::configureLong(configurer, mState, port, "minValue");
	mMaxValue = ConfigurerHelper::configureLong(configurer, mState, port, "maxValue");
	mMinValueScaled = ConfigurerHelper::configureLong(configurer, mState, port, "minValueScaled");
	mMaxValueScaled = ConfigurerHelper::configureLong(configurer, mState, port, "maxValueScaled");

	if (mMinValue == mMaxValue) {
		QLOG_ERROR() << "PWM Capture configuration error: minValue = maxValue!";
		failSetup();
		mK = 0;
		mB = 0;
	} else {
		mK = static_cast<qreal>(mMaxValueScaled - mMinValueScaled) / static_cast<qreal>(mMaxValue - mMinValue);
		mB = static_cast<qreal>(mMinValueScaled) - mK * static_cast<qreal>(mMinValue);
	}

	mState.ready();
}

PwmCapture::~PwmCapture()
{
	mCaptureFile.reset();
}

void PwmCapture::failSetup()
{
	mState.fail();
	mCaptureFile.reset();
	mPortLease.release();
	mPwmChannel.release();
}

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
