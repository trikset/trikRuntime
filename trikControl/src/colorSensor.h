/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include <QtCore/QThread>
#include <QtCore/QScopedPointer>

#include "colorSensorInterface.h"

#include "declSpec.h"

namespace trikControl {

class ColorSensorWorker;

/// Implementation of color sensor for real robot.
class ColorSensor : public ColorSensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param script - file name of a scrit used to start or stop a sensor.
	/// @param inputFile - sensor input fifo. Note that we will write data here, not read it.
	/// @param outputFile - sensor output fifo. Note that we will read sensor data from here.
	/// @param m - horisontal dimension of a sensor.
	/// @param n - vertical dimension of a sensor.
	ColorSensor(QString const &script, QString const &inputFile, QString const &outputFile, int m, int n);

	~ColorSensor() override;

public slots:
	void init(bool showOnDisplay) override;

	QVector<int> read(int m, int n) override;

	void stop() override;

private:
	/// Worker object that handles sensor in separate thread.
	QScopedPointer<ColorSensorWorker> mColorSensorWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}
