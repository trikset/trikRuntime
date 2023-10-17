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

#pragma once

#include <QtCore/QScopedPointer>

#include "pwmCaptureInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class InputDeviceFileInterface;
}

namespace trikControl {

/// Implementation of PWM capture for real robot.
class PwmCapture : public PwmCaptureInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	PwmCapture(const QString &port, const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~PwmCapture() override;

	Status status() const override;

	long minValue() const override;

	long maxValue() const override;

public slots:
	/// Returns three readings of PWM signal frequency.
	QVector<int> frequency() override;

	/// Returns scaled PWM signal duty.
	long duty() override;

	/// Returns raw PWM signal duty.
	long dutyRaw() override;

private:
	QScopedPointer<trikHal::InputDeviceFileInterface> mFrequencyFile;
	QScopedPointer<trikHal::InputDeviceFileInterface> mDutyFile;
	DeviceState mState;
	long mMinValue;
	long mMaxValue;
	long mMinValueScaled;
	long mMaxValueScaled;
	qreal mK;
	qreal mB;
};

}
