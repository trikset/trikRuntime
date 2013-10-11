/* Copyright 2013 Yurii Litvinov
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

#include "sensor.h"

#include <QtCore/QDebug>

using namespace trikControl;

Sensor::Sensor(int min, int max, QString const &deviceFile)
	: mMin(min)
	, mMax(max)
	, mDeviceFile(deviceFile)
{
	if (!mDeviceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "File " << deviceFile << " failed to open for reading";
		return;
	}

	mStream.setDevice(&mDeviceFile);
}

int Sensor::read()
{
	if (mMax == mMin) {
		return mMin;
	}

	int value = 0;
	mStream >> value;

	value = qMin(value, mMax);
	value = qMax(value, mMin);

	double const scale = 100.0 / (static_cast<double>(mMax - mMin));

	value = (value - mMin) * scale;

	return value;
}
