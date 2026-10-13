/* Copyright 2013 - 2016 Yurii Litvinov, Anastasiia Kornilova and CyberTech Labs Ltd.
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
#include <QtCore/QTimer>
#include <QtCore/QVector>

#include "communicatorCommand.h"
#include "configurerHelper.h"
#include "deviceState.h"
#include "motorInterface.h"
#include "peripheryDriverInterface.h"
#include "transports/policies.h"
#include "transports/transportTraits.h"

#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>
#include <QsLog.h>

namespace trikControl {

static const int powerMotorMaxControl = 100;
static const int powerMotorMinControl = -100;

/// TRIK power motor, parametrized by the transport policy.
template<typename Policy>
class PowerMotor;

template<>
struct SupportsCommunicator<PowerMotor> : std::true_type {};

/// Power motor served over the periphery communicator (UART/I2C).
template<>
class PowerMotor<CommunicatorPolicy> : public MotorInterface
{
public:
	/// Constructor.
	/// @param port - port on which this motor is configured.
	/// @param configurer - configurer object containing preparsed XML files with motor parameters.
	/// @param driver - periphery module driver used to control the motor.
	PowerMotor(const QString &port, const trikKernel::Configurer &configurer, PeripheryDriverInterface *driver)
		: mDriver(driver)
		, mInvert(configurer.attributeByPort(port, "invert") == "false")
		, mCurrentPower(0)
		, mState("Power Motor on " + port)
	{
		mMspCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");

		mCurrentPeriod = ConfigurerHelper::configureInt(configurer, mState, port, "period");
		setPeriod(mCurrentPeriod);

		mPowerMap.reserve(powerMotorMaxControl + 1);
		lineariseMotor(port, configurer);

		mState.ready();
	}

	Status status() const override
	{
		return mDriver ? combine(*mDriver, mState.status()) : mState.status();
	}

	int minControl() const override
	{
		return powerMotorMinControl;
	}

	int maxControl() const override
	{
		return powerMotorMaxControl;
	}

	void setPower(int power, bool constrain = true) override
	{
		if (constrain) {
			if (power > powerMotorMaxControl) {
				power = powerMotorMaxControl;
			} else if (power < powerMotorMinControl) {
				power = powerMotorMinControl;
			}
			power = power <= 0 ? -mPowerMap[-power] : mPowerMap[power];
		}

		mCurrentPower = power;

		power = mInvert ? -power : power;

		CommunicatorCommand command;
		command.device = static_cast<uint8_t>(mMspCommandNumber & 0xFF);
		command.data = QByteArray(1, static_cast<char>(power & 0xFF));

		mDriver->send(command);
	}

	int power() const override
	{
		return mCurrentPower;
	}

	void powerOff() override
	{
		setPower(0, false); // ignore power units translation (linearisation)
	}

	void brake(int durationMs = 500) override
	{
		if (durationMs <= 0) {
			brake();
		}

		setPower(0x7f, false);
		QTimer::singleShot(durationMs, this, &PowerMotor::powerOff);
	}

	/// Returns currently set period of a motor.
	int period() const
	{
		return mCurrentPeriod;
	}

	/// Sets current motor period.
	/// Period of pulses is a time interval between two characteristic points of two adjacent pulses.
	void setPeriod(int period) override
	{
		mCurrentPeriod = period;
		CommunicatorCommand command;
		command.device = static_cast<uint8_t>((mMspCommandNumber - 4) & 0xFF);

		QByteArray data(2, '\0');
		data[0] = static_cast<char>(period & 0xFF);
		data[1] = static_cast<char>(period >> 8);
		command.data = data;

		mDriver->send(command);
	}

private:
	void lineariseMotor(const QString &port, const trikKernel::Configurer &configurer)
	{
		QVector<QPair<double, double>> powerGraph;
		for (const QString &str : configurer.attributeByPort(port, "measures").split(")")) {
			if (!str.isEmpty()) {
				QPair<double, double> temp;
				temp.first = str.mid(1).split(";").at(0).toInt();
				temp.second = str.mid(1).split(";").at(1).toInt();
				powerGraph.append(temp);
			}
		}

		const int graphLength = powerGraph.size();
		const int maxValue = powerGraph[graphLength - 1].second;

		for (int i = 0; i < graphLength; i++) {
			powerGraph[i].second *= powerMotorMaxControl;
			powerGraph[i].second /= maxValue;
		}

		for (int i = 0; i < powerMotorMaxControl; i++) {
			int k = 0;
			while (i >= powerGraph[k].second) {
				k++;
			}

			k--;

			const double measureDifference = powerGraph[k + 1].second - powerGraph[k].second;
			const double axeDifferenece = powerGraph[k + 1].first - powerGraph[k].first;

			if (measureDifference < 0 || axeDifferenece < 0) {
				throw trikKernel::MalformedConfigException("Nonmonotonic function");
			}

			const double coef = axeDifferenece / measureDifference;
			const int power = powerGraph[k].first + coef * (i - powerGraph[k].second);
			mPowerMap.append(power);
		}

		mPowerMap.append(powerMotorMaxControl);
	}

	PeripheryDriverInterface *mDriver;
	int mMspCommandNumber = 0;
	const bool mInvert;
	int mCurrentPower = 0;
	int mCurrentPeriod = 0;
	DeviceState mState;
	QVector<int> mPowerMap;
};

} // namespace trikControl

Q_DECLARE_METATYPE(trikControl::PowerMotor<trikControl::CommunicatorPolicy>*)