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

#include <QtCore/QByteArray>
#include <QtCore/QTextStream>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include "configurerHelper.h"

#include <QsLog.h>

using namespace trikControl;

PwmCapture::PwmCapture(const QString &port, const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mFrequencyFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "frequencyFile")))
	, mDutyFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "dutyFile")))
	, mState("PWM Capture on " + port)
{
	if (!mFrequencyFile->open()) {
		mState.fail();
	}

	if (!mDutyFile->open()) {
		mState.fail();
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
		mK = static_cast<qreal>(mMaxValueScaled - mMinValueScaled) / (mMaxValue - mMinValue);
		mB = mMinValueScaled - mK * mMinValue;
	}

	mState.ready();
}

PwmCapture::~PwmCapture()
{
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

	mFrequencyFile->reset();
	QVector<int> data(3);
	char c = '\0';
	mFrequencyFile->stream() >> data[0] >> c >> data[1] >> c >> data[2] >> c;
	return data;
}

long PwmCapture::duty()
{
	if (!mState.isReady()) {
		return {};
	}

	long data = dutyRaw();

	long result = std::min(mMaxValue, std::max(mMinValue, data));
	result = mK * result + mB;

	return result;
}

long PwmCapture::dutyRaw()
{
	if (!mState.isReady()) {
		return {};
	}

	mDutyFile->reset();
	long data = 0;
	char c = '\0';
	mDutyFile->stream() >> data >> c;
	return data;
}

long PwmCapture::minValue() const
{
	return mMinValueScaled;
}

long PwmCapture::maxValue() const
{
	return mMaxValueScaled;
}
