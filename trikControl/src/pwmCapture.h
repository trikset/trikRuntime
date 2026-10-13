/* Copyright 2013 - 2015 CyberTech Labs Ltd.
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

#pragma once

#include <algorithm>

#include <QtCore/QByteArray>
#include <QtCore/QScopedPointer>
#include <QtCore/QTextStream>

#include "configurerHelper.h"
#include "deviceState.h"
#include "pwmCaptureInterface.h"
#include "transports/policies.h"
#include "transports/transportTraits.h"

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/configurer.h>
#include <QsLog.h>

namespace trikHal {
class InputDeviceFileInterface;
}

namespace trikControl {

/// Implementation of PWM capture for real robot, parametrized by the transport policy.
template<typename Policy>
class PwmCapture;

template<>
struct SupportsInternal<PwmCapture> : std::true_type {};

/// PWM capture served by the on-module hardware via sysfs files.
template<>
class PwmCapture<InternalPolicy> : public PwmCaptureInterface
{
public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	PwmCapture(const QString &port, const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
		: mFrequencyFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "frequencyFile")))
		, mDutyFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "dutyFile")))
		, mState("PWM Capture on " + port)
	{
		QScopedPointer<trikHal::OutputDeviceFileInterface> captureFile
				(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByPort(port, "captureFile")));
		if (!captureFile->open()) {
			mState.fail();
			return;
		}
		captureFile->write(QString::number(1));

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

	~PwmCapture() override = default;

	Status status() const override
	{
		return mState.status();
	}

	/// Returns three readings of PWM signal frequency.
	QVector<int> frequency() override
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

	/// Returns scaled PWM signal duty.
	long duty() override
	{
		if (!mState.isReady()) {
			return {};
		}

		long data = dutyRaw();

		long result = std::min(mMaxValue, std::max(mMinValue, data));
		result = mK * result + mB;

		return result;
	}

	/// Returns raw PWM signal duty.
	long dutyRaw() override
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

	long minValue() const override
	{
		return mMinValueScaled;
	}

	long maxValue() const override
	{
		return mMaxValueScaled;
	}

private:
	QScopedPointer<trikHal::InputDeviceFileInterface> mFrequencyFile;
	QScopedPointer<trikHal::InputDeviceFileInterface> mDutyFile;
	DeviceState mState;
	long mMinValue = 0;
	long mMaxValue = 0;
	long mMinValueScaled = 0;
	long mMaxValueScaled = 0;
	qreal mK = 0;
	qreal mB = 0;
};

} // namespace trikControl

Q_DECLARE_METATYPE(trikControl::PwmCapture<trikControl::InternalPolicy>*)