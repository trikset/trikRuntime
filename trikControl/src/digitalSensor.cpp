/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "digitalSensor.h"

#include <QtCore/QDebug>

#include <trikKernel/configurer.h>

#include <QsLog.h>

using namespace trikControl;

DigitalSensor::DigitalSensor(QString const &port, trikKernel::Configurer const &configurer)
	: mMin(configurer.attributeByPort(port, "min").toInt())
	, mMax(configurer.attributeByPort(port, "max").toInt())
	, mDeviceFile(configurer.attributeByPort(port, "deviceFile"))
{
}

int DigitalSensor::read()
{
	if (!mDeviceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QLOG_ERROR() << "File " << mDeviceFile.fileName() << " failed to open for reading";
		qDebug() << "File " << mDeviceFile.fileName() << " failed to open for reading";
		return 0;
	}

	mStream.setDevice(&mDeviceFile);

	if (mMax == mMin) {
		return mMin;
	}

	int value = 0;
	mStream >> value;

	mDeviceFile.close();

	value = qMin(value, mMax);
	value = qMax(value, mMin);

	double const scale = 100.0 / (static_cast<double>(mMax - mMin));

	value = (value - mMin) * scale;

	return value;
}

int DigitalSensor::readRawData()
{
	if (!mDeviceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QLOG_ERROR() << "File " << mDeviceFile.fileName() << " failed to open for reading";
		qDebug() << "File " << mDeviceFile.fileName() << " failed to open for reading";
		return 0;
	}

	mStream.setDevice(&mDeviceFile);

	int value = 0;
	mStream >> value;

	mDeviceFile.close();

	return value;
}
