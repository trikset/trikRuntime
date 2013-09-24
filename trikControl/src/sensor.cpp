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

#include <QtCore/QTextStream>

#include <QtCore/QDebug>

using namespace trikControl;

Sensor::Sensor()
	: mMin(0)
	, mMax(0)
{
}

void Sensor::init(int min, int max, QString const &controlFile)
{
	mMin = min;
	mMax = max;
	mControlFile.setFileName(controlFile);
}

int Sensor::read()
{
	if (mMax == mMin) {
		return mMin;
	}

	if (mControlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream stream(&mControlFile);
		int value = 0;
		stream >> value;
		mControlFile.close();

		qDebug() << "read, raw reading: " << value;

		value = qMin(value, mMax);
		value = qMax(value, mMin);

		double const scale = 100.0 / (static_cast<double>(mMax) - static_cast<double>(mMin));

		qDebug() << "read, scale: " << scale << "mMin" << mMin << "mMax" << mMax;

		value = (value - mMin) * scale;

		qDebug() << "read, normalized reading: " << value;

		return value;
	}

	qDebug() << "read, reading failed, file name: " << mControlFile.fileName();

	return 0;
}
