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

#include "powerMotor.h"

#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>
#include <trikKernel/exceptions/internalErrorException.h>
#include <QTimer>
#include "mspI2cCommunicator.h"
#include "configurerHelper.h"

using namespace trikControl;

static const int maxControlValue = 100;
static const int minControlValue = -100;

PowerMotor::PowerMotor(const QString &port, const trikKernel::Configurer &configurer
		, MspCommunicatorInterface &communicator)
	: mCommunicator(communicator)
	, mInvert(configurer.attributeByPort(port, "invert") == "false")
	, mCurrentPower(0)
	, mState("Power Motor on" + port)
{
	mMspCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");

	mCurrentPeriod = ConfigurerHelper::configureInt(configurer, mState, port, "period");
	setPeriod(mCurrentPeriod);

	mPowerMap.reserve(maxControlValue + 1);
	lineariseMotor(port, configurer);

	mState.ready();
}

PowerMotor::~PowerMotor()
{
}

PowerMotor::Status PowerMotor::status() const
{
	return combine(mCommunicator, mState.status());
}

void PowerMotor::setPower(int power, bool constrain)
{
	if (constrain) {
		if (power > maxControlValue) {
			power = maxControlValue;
		} else if (power < minControlValue) {
			power = minControlValue;
		}
		power = power <= 0 ? -mPowerMap[-power] : mPowerMap[power];
	}

	mCurrentPower = power;


	power = mInvert ? -power : power;

	QByteArray command(3, '\0');
	command[0] = static_cast<char>(mMspCommandNumber & 0xFF);
	command[1] = static_cast<char>((mMspCommandNumber >> 8) & 0xFF);
	command[2] = static_cast<char>(power & 0xFF);

	mCommunicator.send(command);
}

int PowerMotor::power() const
{
	return mCurrentPower;
}

int PowerMotor::period() const
{
	return mCurrentPeriod;
}

void PowerMotor::powerOff()
{
	setPower(0, false); // ignore power units translation (linearisation)
}

void PowerMotor::forceBreak(int durationMs)
{
	if (durationMs <= 0)
		forceBreak();
	setPower(0x7f, false);
	QTimer::singleShot(durationMs, this, SLOT(powerOff()));
}

void PowerMotor::setPeriod(int period)
{
	mCurrentPeriod = period;
	QByteArray command(4, '\0');
	command[0] = static_cast<char>((mMspCommandNumber - 4) & 0xFF);
	command[2] = static_cast<char>(period & 0xFF);
	command[3] = static_cast<char>(period >> 8);
	mCommunicator.send(command);
}

void PowerMotor::lineariseMotor(const QString &port, const trikKernel::Configurer &configurer)
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
		powerGraph[i].second *= maxControlValue;
		powerGraph[i].second /= maxValue;
	}

	for (int i = 0; i < maxControlValue; i++) {
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

	mPowerMap.append(maxControlValue);
}

int PowerMotor::minControl() const
{
	return minControlValue;
}

int PowerMotor::maxControl() const
{
	return maxControlValue;
}
