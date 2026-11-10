/* Copyright 2013 - 2026 Yurii Litvinov, Anastasiia Kornilova and CyberTech Labs Ltd.
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

#include <QtCore/QTimer>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/inputDeviceFileInterface.h>
#include <trikHal/outputDeviceFileInterface.h>
#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "configurerHelper.h"
#include "port/iioDeviceResolver.h"
#include "port/portManager.h"

using namespace trikControl;

PowerMotor::~PowerMotor() = default;

PowerMotor::PowerMotor(const QString &port, const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hw, PortManager &ports)
	: mPortLease(ports.acquire(port, PortMode::Pwm, QStringLiteral("powerMotor")))
	, mState("Power Motor on " + port)
{
	if (!mPortLease) {
		QLOG_ERROR() << "PowerMotor on" << port << "cannot acquire PWM port";
		mState.fail();
		return;
	}

	mPwmChannel = PwmChannelLease(mPortLease.device(), hw);
	if (!mPwmChannel) {
		QLOG_ERROR() << "PowerMotor on" << port << "cannot resolve PWM device" << mPortLease.device();
		mState.fail();
		return;
	}

	const auto channelDir = mPwmChannel.channelDir();
	const auto dutyPath = channelDir + QStringLiteral("/duty_cycle");
	const auto periodPath = channelDir + QStringLiteral("/period");
	const auto runPath = channelDir + QStringLiteral("/enable");
	QString dirPath;
	dirPath = configurer.attributeByPort(port, QStringLiteral("dirGpio"), &dirPath);
	if (dirPath.isEmpty()) {
		dirPath = configurer.attributeByPort(port, QStringLiteral("dirFile"));
	}

	mDirLine = GpioLineLease(dirPath, GpioLineLease::Direction::Out, false, hw);
	if (!mDirLine) {
		QLOG_ERROR() << "PowerMotor on" << port << "cannot prepare dir GPIO" << dirPath;
		mState.fail();
		return;
	}
	dirPath = mDirLine.valueFile();

	mDutyFile.reset(hw.createOutputDeviceFile(dutyPath));
	mPeriodFile.reset(hw.createOutputDeviceFile(periodPath));
	mRunFile.reset(hw.createOutputDeviceFile(runPath));
	mDirFile.reset(hw.createOutputDeviceFile(dirPath));

	if (!mDutyFile || !mPeriodFile || !mRunFile || !mDirFile) {
		QLOG_ERROR() << "PowerMotor on" << port << "requires a valid PWM channel and dirFile";
		mState.fail();
		return;
	}

	const int periodUs = ConfigurerHelper::configureInt(configurer, mState, port, "period");
	if (periodUs <= 0) {
		QLOG_ERROR() << "PowerMotor on" << port << "has invalid period:" << periodUs;
		mState.fail();
		return;
	}
	mPeriodNs = periodUs * 1000;

	auto openFile = [&](auto &file, const QString &path) {
		if (!file->open()) {
			QLOG_ERROR() << "PowerMotor on" << port << "failed to open" << path;
			mState.fail();
			return false;
		}

		return true;
	};

	if (!openFile(mPeriodFile, periodPath) || !openFile(mRunFile, runPath) || !openFile(mDutyFile, dutyPath)
		|| !openFile(mDirFile, dirPath)) {
		return;
	}

	mPeriodFile->write(QString::number(mPeriodNs));
	mRunFile->write(QString::number(0));
	mDutyFile->write(QString::number(0));

	QString senseLabel;
	QString senseChannel;
	senseLabel = configurer.attributeByPort(port, "iioLabel", &senseLabel);
	senseChannel = configurer.attributeByPort(port, "iioChannel", &senseChannel);

	if (!senseLabel.isEmpty() && !senseChannel.isEmpty()) {
		const auto senseFile = findIioChannelFileByLabel(senseLabel, senseChannel);
		if (senseFile.isEmpty()) {
			QLOG_WARN() << "PowerMotor on" << port << "cannot resolve sense channel" << senseLabel
				    << senseChannel;
		} else {
			mSenseFile.reset(hw.createInputDeviceFile(senseFile));
			if (!mSenseFile || !mSenseFile->open()) {
				QLOG_WARN() << "PowerMotor on" << port << "cannot open sense file" << senseFile;
				mSenseFile.reset();
			} else {
				auto scaleFile = senseFile;
				scaleFile.replace(QStringLiteral("_raw"), QStringLiteral("_scale"));
				std::unique_ptr<trikHal::InputDeviceFileInterface> scaleFileHandle(
					hw.createInputDeviceFile(scaleFile));
				if (scaleFileHandle && scaleFileHandle->open()) {
					auto scale = 0.0;
					scaleFileHandle->stream() >> scale;
					scaleFileHandle->close();
					if (scale > 0.0) {
						mSenseScaleMv = scale;
					}
				} else {
					QLOG_WARN() << "PowerMotor on" << port << "cannot open" << scaleFile
						    << ", using default sense scale";
				}
			}
		}
	} else {
		QLOG_WARN() << "PowerMotor on" << port << "has no sense channel configured";
	}

	QString faultPath;
	faultPath = configurer.attributeByPort(port, QStringLiteral("nFaultGpio"), &faultPath);
	if (faultPath.isEmpty()) {
		faultPath = configurer.attributeByPort(port, QStringLiteral("nFault"), &faultPath);
	}

	if (!faultPath.isEmpty()) {
		mFaultLine = GpioLineLease(faultPath, GpioLineLease::Direction::In, false, hw);
		if (!mFaultLine) {
			QLOG_WARN() << "PowerMotor on" << port << "cannot prepare nFault GPIO" << faultPath;
		} else {
			faultPath = mFaultLine.valueFile();
		}

		mFaultFile.reset(hw.createInputDeviceFile(faultPath));
		if (!mFaultFile || !mFaultFile->open()) {
			QLOG_WARN() << "PowerMotor on" << port << "cannot open nFault file" << faultPath;
			mFaultFile.reset();
		}
	} else {
		QLOG_WARN() << "PowerMotor on" << port << "has no nFault file configured";
	}

	mPowerMap.reserve(powerMotorMaxControl + 1);
	lineariseMotor(port, configurer);

	mInvert = configurer.attributeByPort(port, "invert") == "false";
	mState.ready();
}

PowerMotor::Status PowerMotor::status() const
{
	return mState.status();
}

int PowerMotor::minControl() const
{
	return powerMotorMinControl;
}

int PowerMotor::maxControl() const
{
	return powerMotorMaxControl;
}

// NOLINTNEXTLINE(google-default-arguments)
void PowerMotor::setPower(int power, bool constrain)
{
	if (!mState.isReady() || !mRunFile || !mDutyFile || !mDirFile) {
		return;
	}

	if (constrain) {
		if (power > powerMotorMaxControl) {
			power = powerMotorMaxControl;
		} else if (power < powerMotorMinControl) {
			power = powerMotorMinControl;
		}
	}

	mCurrentPower = power;
	power = mInvert ? -power : power;

	int mapPower = power;
	if (mapPower > powerMotorMaxControl) {
		mapPower = powerMotorMaxControl;
	} else if (mapPower < powerMotorMinControl) {
		mapPower = powerMotorMinControl;
	}

	const int pct = mapPower >= 0 ? mPowerMap[mapPower] : mPowerMap[-mapPower];
	const int dutyNs = mPeriodNs * pct / 100;

	mRunFile->write(QString::number(0));
	mDirFile->write(QString::number(power >= 0 ? 1 : 0));
	mDutyFile->write(QString::number(dutyNs));
	mRunFile->write(QString::number(1));
}

int PowerMotor::power() const
{
	return mCurrentPower;
}

void PowerMotor::powerOff()
{
	mCurrentPower = 0;

	if (!mRunFile || !mDutyFile) {
		return;
	}

	mRunFile->write(QString::number(0));
	mDutyFile->write(QString::number(0));
}

// NOLINTNEXTLINE(google-default-arguments)
void PowerMotor::brake(int durationMs)
{
	if (durationMs <= 0) {
		durationMs = 500;
	}

	setPower(0, false);
	QTimer::singleShot(durationMs, this, &PowerMotor::powerOff);
}

void PowerMotor::setPeriod(int uSec)
{
	if (uSec <= 0) {
		QLOG_ERROR() << "PowerMotor has invalid period:" << uSec;
		mState.fail();
		return;
	}

	if (!mPeriodFile) {
		QLOG_ERROR() << "PowerMotor has no period file";
		mState.fail();
		return;
	}

	mPeriodNs = uSec * 1000;
	mPeriodFile->write(QString::number(mPeriodNs));
}

void PowerMotor::lineariseMotor(const QString &port, const trikKernel::Configurer &configurer)
{
	QVector<QPair<double, double>> powerGraph;
	for (const QString &str : configurer.attributeByPort(port, "measures").split(")")) {
		if (!str.isEmpty()) {
			QPair<double, double> t;
			t.first = str.mid(1).split(";").at(0).toInt();
			t.second = str.mid(1).split(";").at(1).toInt();
			powerGraph.append(t);
		}
	}

	if (powerGraph.isEmpty()) {
		for (int i = 0; i <= powerMotorMaxControl; ++i) {
			mPowerMap.append(i);
		}
		return;
	}

	const int gl = powerGraph.size();
	if (gl < 2) {
		QLOG_ERROR() << "PowerMotor on" << port << "requires at least two measure points";
		mState.fail();
		return;
	}

	const double maxV = powerGraph[gl - 1].second;
	if (maxV <= 0.0) {
		QLOG_ERROR() << "PowerMotor on" << port << "measure range must be positive";
		mState.fail();
		return;
	}

	for (int i = 0; i < gl; i++) {
		powerGraph[i].second *= powerMotorMaxControl;
		powerGraph[i].second /= maxV;
	}
	for (int i = 0; i < powerMotorMaxControl; i++) {
		int k = 0;
		while (i >= powerGraph[k].second) {
			k++;
		}
		k--;
		const double md = powerGraph[k + 1].second - powerGraph[k].second;
		const double ad = powerGraph[k + 1].first - powerGraph[k].first;
		if (md <= 0 || ad < 0) {
			QLOG_ERROR() << "PowerMotor on" << port << "measure function is not monotonic";
			mState.fail();
			return;
		}
		mPowerMap.append(static_cast<int>(powerGraph[k].first + ad / md * (i - powerGraph[k].second)));
	}
	mPowerMap.append(powerMotorMaxControl);
}

int PowerMotor::readRawSense()
{
	if (!mState.isReady() || !mSenseFile) {
		return 0;
	}

	mSenseFile->reset();

	auto raw = 0;
	mSenseFile->stream() >> raw;
	return raw;
}

float PowerMotor::readSense()
{
	return static_cast<float>(readRawSense() * mSenseScaleMv / 1000.0);
}

bool PowerMotor::hasFault() const
{
	if (!mFaultFile) {
		return false;
	}

	mFaultFile->reset();

	auto value = 1;
	mFaultFile->stream() >> value;
	return value == 0;
}
