/* Copyright 2026 CyberTech Labs Ltd.
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

#include "magnetometerSensor.h"

#include <array>
#include <cmath>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include <QsLog.h>

using namespace trikControl;

MagnetometerSensor::MagnetometerSensor(const QString &port, const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState(QStringLiteral("magnetometer"))
{
	qRegisterMetaType<trikKernel::TimeVal>("trikKernel::TimeVal");

	const auto deviceFile = configurer.attributeByPort(port, "deviceFile");

	mIioFile.reset(hardwareAbstraction.createIIOFile(deviceFile));
	if (!mIioFile->open()) {
		QLOG_ERROR() << "Magnetometer: failed to open" << deviceFile;
		mState.fail();
		return;
	}

	auto error = QString();
	if (!validateLayout(&error)) {
		QLOG_ERROR() << "Magnetometer: layout validation failed for" << deviceFile << ":" << error;
		mIioFile->close();
		mIioFile.reset();
		mState.fail();
		return;
	}

	connect(mIioFile.data(), &trikHal::IIOFileInterface::newData, this, &MagnetometerSensor::onNewData);

	mCalibrationTimer.setSingleShot(true);
	connect(&mCalibrationTimer, &QTimer::timeout, this, [this]() {
		{
			QMutexLocker locker(&mCalibrationLock);
			mCalibrating = false;
		}

		calibrateInternal();
	});

	mState.ready();
	QLOG_INFO() << "Starting MagnetometerSensor on" << deviceFile;
}

MagnetometerSensor::~MagnetometerSensor()
{
	if (mIioFile && mIioFile->isOpened()) {
		mIioFile->close();
	}
}

DeviceInterface::Status MagnetometerSensor::status() const
{
	return mState.status();
}

QVector<int> MagnetometerSensor::read() const
{
	QReadLocker locker(&mResultLock);
	return mResult;
}

QVector<int> MagnetometerSensor::readRawData() const
{
	QReadLocker locker(&mResultLock);
	return mRaw;
}

void MagnetometerSensor::calibrate(int msec)
{
	{
		QMutexLocker locker(&mCalibrationLock);
		mCollectedSamples = 0;
		mCalibrating = true;
	}

	QMetaObject::invokeMethod(&mCalibrationTimer, [this, msec]() { mCalibrationTimer.start(msec); });
	QLOG_INFO() << "Magnetometer: collecting calibration samples";
}

/// Calibration model and plan.
///
/// The magnetometer is affected by two kinds of static magnetic disturbances:
///  - hard iron: a permanent magnetic field fixed relative to the sensor, e.g.
///    permanent magnets, magnetized screws or iron parts, DC currents in board
///    traces, and the sensor offset. It adds a constant vector and shifts the
///    centre of the measured cloud;
///  - soft iron: induced magnetization of nearby ferromagnetic materials, e.g.
///    steel screws/standoffs, shields, inductor/transformer cores, steel battery
///    cans. It distorts the field linearly, stretching the sphere into an
///    ellipsoid.
///
/// Without disturbances the projections of the Earth's magnetic field onto the
/// sensor axes, sampled over all orientations, would lie on a sphere of radius
/// F (normalized to 1). Under hard iron this sphere is shifted linearly; under
/// soft iron it is stretched and turns into an ellipsoid.
///
/// Time-varying strong currents (for example motor currents) are not modelled
/// here. This matters for many applications, but in principle the field as a
/// function of motor duty cycle / load could be measured in advance and the
/// corresponding vectors subtracted at runtime (the board has current sensors
/// for the motors). For now this is out of scope.
///
/// A general second-order surface (ellipsoid) is written as
///
///   S: a*x^2 + b*y^2 + c*z^2 + 2f*y*z + 2g*x*z + 2h*x*y
///      + p*x + q*y + r*z + d = 0,
///
/// or, in matrix form,
///
///   S: x^T*M*x + x^T*n + d = 0,
///
/// where x = [x, y, z]^T, M is a symmetric 3x3 matrix of quadratic
/// coefficients, n is a 3-vector of linear coefficients and d is the constant
/// coefficient. The first step of calibration is to find M, n and d. The next
/// step is to find the displacement b (the ellipsoid centre) such that, when
/// substituted as x = y + b, it removes the linear part of the equation.
///
/// After centering, the equation becomes (x - b)^T M (x - b) = k^2. The final
/// step is to find a matrix A that maps this centred ellipsoid onto a sphere of
/// the target radius T, i.e. ||A (x - b)|| = T. This is equivalent to
/// A^T A = (T^2 / k^2) M, so A is a scaled matrix square root of M. It can be
/// obtained either from a Cholesky decomposition M = L L^T (A = (T/k) L^T) or
/// from a symmetric eigendecomposition (A = (T/k) sqrt(M)); the two differ by a
/// rotation, which does not matter for a sphere.
void MagnetometerSensor::calibrateInternal()
{
	QVector<double> result {};
	setCalibrationValues(result);
}

void MagnetometerSensor::setCalibrationValues(const QVector<double> &values)
{
	QWriteLocker locker(&mResultLock);
	mCalibrationValues = values;
	mIsCalibrated = values.size() >= 12;
}

QVector<double> MagnetometerSensor::getCalibrationValues()
{
	QReadLocker locker(&mResultLock);
	return mCalibrationValues;
}

bool MagnetometerSensor::isCalibrated() const
{
	QReadLocker locker(&mResultLock);
	return mIsCalibrated;
}

bool MagnetometerSensor::validateLayout(QString *error)
{
	const auto channels = mIioFile->channels();

	const auto find = [&channels](trikHal::IIOChanType type,
				  trikHal::IIOAxis axis) -> const trikHal::IIOChannelInfo * {
		for (const auto &channel : channels) {
			if (channel.id.type == type && channel.id.axis == axis) {
				return &channel;
			}
		}

		return nullptr;
	};

	const std::array<trikHal::IIOAxis, 3> axes {trikHal::IIOAxis::X, trikHal::IIOAxis::Y, trikHal::IIOAxis::Z};

	auto reference = trikHal::IIOChannelFormat {};
	for (auto i = 0; i < 3; ++i) {
		const auto *channel = find(trikHal::IIOChanType::Magn, axes[i]);
		if (!channel) {
			*error = QStringLiteral("missing magnetometer channel");
			return false;
		}

		if (channel->format.storageBits != 16) {
			*error = QStringLiteral("magnetometer channels must be 16-bit");
			return false;
		}

		if (i == 0) {
			reference = channel->format;
		} else if (channel->format != reference) {
			*error = QStringLiteral("magnetometer channels have different scan types");
			return false;
		}

		mValueIndex[i] = mIioFile->channelValueIndex(trikHal::IIOChanType::Magn, axes[i]);
		if (mValueIndex[i] < 0) {
			*error = QStringLiteral("magnetometer channel is not exposed by the reader");
			return false;
		}
	}

	return true;
}

bool MagnetometerSensor::storeCalibrationSample(const QVector<int> &raw)
{
	if (mCollectedSamples >= kCalibrationSamples) {
		return false;
	}

	if (mCollectedSamples > 0) {
		const auto &last = mCalibrationSamples.at(mCollectedSamples - 1);
		const auto dx = raw.at(0) - last.x;
		const auto dy = raw.at(1) - last.y;
		const auto dz = raw.at(2) - last.z;

		if (dx * dx + dy * dy + dz * dz < kCalibrationMinDelta * kCalibrationMinDelta) {
			return false;
		}
	}

	mCalibrationSamples[mCollectedSamples++] = CalibrationSample {raw.at(0), raw.at(1), raw.at(2)};
	return true;
}

QVector<int> MagnetometerSensor::applyCalibration(const QVector<int> &raw) const
{
	if (!mIsCalibrated || raw.size() != 3 || mCalibrationValues.size() < 12) {
		return raw;
	}

	const auto x = static_cast<double>(raw.at(0)) - mCalibrationValues.at(0);
	const auto y = static_cast<double>(raw.at(1)) - mCalibrationValues.at(1);
	const auto z = static_cast<double>(raw.at(2)) - mCalibrationValues.at(2);

	const auto *a = mCalibrationValues.constData() + 3;
	const auto correctedX = a[0] * x + a[1] * y + a[2] * z;
	const auto correctedY = a[3] * x + a[4] * y + a[5] * z;
	const auto correctedZ = a[6] * x + a[7] * y + a[8] * z;

	return {
		static_cast<int>(std::lround(correctedX)),
		static_cast<int>(std::lround(correctedY)),
		static_cast<int>(std::lround(correctedZ)),
	};
}

void MagnetometerSensor::onNewData(const trikHal::IIOSample &sample)
{
	const auto size = sample.values.size();
	for (auto i = 0; i < 3; ++i) {
		if (mValueIndex[i] < 0 || mValueIndex[i] >= size) {
			QLOG_WARN() << "Magnetometer: unexpected sample size:" << size;
			return;
		}
	}

	const QVector<int> raw {
		sample.values.at(mValueIndex[0]),
		sample.values.at(mValueIndex[1]),
		sample.values.at(mValueIndex[2]),
	};

	{
		QMutexLocker locker(&mCalibrationLock);
		if (mCalibrating) {
			storeCalibrationSample(raw);

			if (mCollectedSamples >= kCalibrationSamples) {
				mCalibrating = false;
			}
		}
	}

	auto result = QVector<int>();
	{
		QWriteLocker locker(&mResultLock);
		mRaw = raw;
		mResult = applyCalibration(raw);
		result = mResult;
	}

	Q_EMIT newData(result, sample.time);
}
