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

#include "colorSensor.h"

#include <algorithm>

#include <trikKernel/configurer.h>

#include "configurerHelper.h"

using namespace trikControl;

ColorSensor::ColorSensor(const QString &port, const trikKernel::Configurer &configurer)
	: m("Color Sensor on " + port, configurer, port, trikDsp::Algorithm::Mxn)
{
	mM = ConfigurerHelper::configureChildInt(
				       configurer, m.state(), port, "colorSensor", "m");
	mN = ConfigurerHelper::configureChildInt(
				       configurer, m.state(), port, "colorSensor", "n");

	// The DSP firmware supports at most a 3x3 grid (COLORS_WIDTHM_MAX /
	// COLORS_HEIGHTN_MAX), so anything larger can never be filled.
	if (mM <= 0 || mM > 3 || mN <= 0 || mN > 3 || m.state().isFailed()) {
		m.state().fail();
		return;
	}

	m.state().ready();

	m.inArgs().m = mM;
	m.inArgs().n = mN;

	mReading.resize(mM);
	for (int i = 0; i < mM; ++i) {
		mReading[i].resize(mN);
		for (int j = 0; j < mN; ++j)
			mReading[i][j] = {0, 0, 0};
	}
}

ColorSensor::~ColorSensor()
{
	Q_EMIT stopped();
}

ColorSensor::Status ColorSensor::status() const
{
	return m.state().status();
}

void ColorSensor::init(bool showOnDisplay)
{
	if (!m.doInit(showOnDisplay))
		return;

	Q_EMIT activateRequested(m.inArgs(), showOnDisplay, true);
}

QVector<int> ColorSensor::read(int mIdx, int nIdx)
{
	QReadLocker locker(&mReadingLock);
	if (mReading.isEmpty() || mIdx <= 0 || nIdx <= 0
	    || mIdx > mReading.size() || nIdx > mReading[0].size()) {
		QLOG_WARN() << QString("Incorrect parameters for read: m = %1, n = %2").arg(mIdx).arg(nIdx);
		return {-1, -1, -1};
	}

	return mReading[mIdx - 1][nIdx - 1];
}

void ColorSensor::stop(int flags) // NOLINT(google-default-arguments)
{
	m.doStop();
	Q_EMIT stopRequested(flags);
	Q_EMIT stopped();
}

void ColorSensor::onResult(const trikDsp::OutArgs &result)
{
	QWriteLocker locker(&mReadingLock);
	const auto total = std::min(mM * mN, 9);
	for (int i = 0; i < total; ++i) {
		const auto row = i / mN;
		const auto col = i % mN;
		const auto c = result.colors[i];
		mReading[row][col] = {
			static_cast<int>((c >> 16) & 0xFF),
			static_cast<int>((c >> 8) & 0xFF),
			static_cast<int>(c & 0xFF)
		};
	}
}

