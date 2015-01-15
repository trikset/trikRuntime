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

#pragma once

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "sensorInterface.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

/// Generic TRIK sensor.
class DigitalSensor : public SensorInterface
{
	Q_OBJECT

public:
	DigitalSensor(QString const &port, trikKernel::Configurer const &configurer);

public slots:
	int read() override;

	int readRawData() override;

private:
	int mMin;
	int mMax;
	QFile mDeviceFile;
	QTextStream mStream;
};

}
