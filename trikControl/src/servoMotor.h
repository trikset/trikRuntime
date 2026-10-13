/* Copyright 2013 - 2014 Yurii Litvinov, CyberTech Labs Ltd.
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

#include <QtCore/QString>
#include <QtCore/QScopedPointer>

#include "configurerHelper.h"
#include "deviceState.h"
#include "motorInterface.h"
#include "transports/policies.h"
#include "transports/transportTraits.h"

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/configurer.h>
#include <QsLog.h>

namespace trikHal {
class OutputDeviceFileInterface;
}

namespace trikControl {

/// TRIK servomotor, parametrized by the transport policy.
template<typename Policy>
class ServoMotor;

template<>
struct SupportsInternal<ServoMotor> : std::true_type {};

/// Servomotor driven by the on-module PWM via sysfs files.
template<>
class ServoMotor<InternalPolicy> : public MotorInterface
{
public:
	/// Constructor.
	/// @param port - port on which this motor is configured.
	/// @param configurer - configurer object containing preparsed XML files with motor parameters.
	ServoMotor(const QString &port, const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
		: mDutyFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByPort(port, "deviceFile")))
		, mPeriodFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByPort(port, "periodFile")))
		, mRunFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByPort(port, "runFile")))
		, mCurrentDutyPercent(0)
		, mInvert(configurer.attributeByPort(port, "invert") == "true")
		, mCurrentPower(0)
		, mRun(false)
		, mState("Servomotor on " + port)
	{
		QString captureFileAttribute = "";
		captureFileAttribute = configurer.attributeByPort(port, "captureFile", &captureFileAttribute);
		if (!captureFileAttribute.isEmpty()) {
			QScopedPointer<trikHal::OutputDeviceFileInterface> captureFile
					(hardwareAbstraction.createOutputDeviceFile(captureFileAttribute));
			if (!captureFile->open()) {
				mState.fail();
				return;
			}
			captureFile->write(QString::number(0));
		}

		const auto configure = [this, &port, &configurer](const QString &parameterName) {
			return ConfigurerHelper::configureInt(configurer, mState, port, parameterName);
		};

		mPeriod = configure("period");
		mMin = configure("min");
		mMax = configure("max");
		mZero = configure("zero");
		mStop = configure("stop");
		mMinControlRange = configure("controlMin");
		mMaxControlRange = configure("controlMax");

		if (!mPeriodFile->open()) {
			mState.fail();
			return;
		}

		if (!mRunFile->open()) {
			mState.fail();
			return;
		} else {
			mRunFile->write(QString::number(mRun ? 1 : 0));
		}

		if (!mDutyFile->open()) {
			mState.fail();
			return;
		} else {
			mDutyFile->write(QString::number(0));
			mState.ready();
		}

		setPeriod(mPeriod / 1000);
	}

	~ServoMotor() override = default;

	Status status() const override
	{
		return mState.status();
	}

	int minControl() const override
	{
		return mMinControlRange;
	}

	int maxControl() const override
	{
		return mMaxControlRange;
	}

	int power() const override
	{
		return mCurrentPower;
	}

	/// Returns currently set frequency of PWM signal supplied to the motor.
	int frequency() const
	{
		return 1000000000.0 / static_cast<qreal>(mPeriod);
	}

	/// Returns currently set duty of PWM signal supplied to the motor.
	int duty() const
	{
		return mCurrentDutyPercent;
	}

	void powerOff() override
	{
		if (!mState.isReady()) {
			QLOG_ERROR() << "Trying to power off motor which is not ready, ignoring";
			return;
		}

		mRunFile->write(QString::number(0));
		mDutyFile->write(QString::number(mStop));
		mRun = false;
		mCurrentPower = 0;
	}

	/// For angular servos sets current motor angle to specified value. For continious rotation servos sets power of
	/// a motor.
	/// @param power - for angular servos --- servo shaft angle, allowed values are from -90 to 90, for continious
	///        rotation servos --- power, allowed values are from -100 to 100.
	/// @param constrain - if true, power will be constrained in an interval [-100, 100], if false, "power" value
	///        will be sent to a motor unaltered.
	void setPower(int power, bool constrain = true) override
	{
		if (!mState.isReady()) {
			QLOG_ERROR() << "Trying to turn on motor which is not ready, ignoring";
			return;
		}

		if (constrain) {
			if (power > mMaxControlRange) {
				power = mMaxControlRange;
			} else if (power < mMinControlRange) {
				power = mMinControlRange;
			}
		}

		mCurrentPower = power;

		const int meanControlRange = (mMaxControlRange + mMinControlRange) / 2;

		power = (mInvert ? -1 : 1) * (power - meanControlRange) + meanControlRange;

		const int range = power <= meanControlRange ? mZero - mMin : mMax - mZero;

		const qreal powerFactor = static_cast<qreal>(range) / (mMaxControlRange - mMinControlRange) * 2;
		const int duty = static_cast<int>(mZero + (power - meanControlRange) * powerFactor);
		const QString command = QString::number(duty);

		mCurrentDutyPercent = 100 * duty / mPeriod;

		if (!mRun) {
			mRun = true;
			mRunFile->write(QString::number(mRun));
		}

		// After switching from capture mode to servo mode,
		// a faster response occurs if you set duty after run.
		mDutyFile->write(command);
	}

	void brake(int /* durationMs */ = 0) override { /** Makes no sense, neither for angular servo nor for rotational */ }

	void setPeriod(int uSec = 20000) override
	{
		const QString command = QString::number(uSec * 1000);
		mPeriodFile->write(command);
	}

private:
	QScopedPointer<trikHal::OutputDeviceFileInterface> mDutyFile;
	QScopedPointer<trikHal::OutputDeviceFileInterface> mPeriodFile;
	QScopedPointer<trikHal::OutputDeviceFileInterface> mRunFile;
	int mPeriod = 0;
	int mCurrentDutyPercent;
	int mMin = 0;
	int mMax = 0;
	int mZero = 0;
	int mStop = 0;
	int mMinControlRange = 0;
	int mMaxControlRange = 0;
	bool mInvert = false;
	int mCurrentPower;
	bool mRun = false;
	DeviceState mState;
};

} // namespace trikControl

Q_DECLARE_METATYPE(trikControl::ServoMotor<trikControl::InternalPolicy>*)