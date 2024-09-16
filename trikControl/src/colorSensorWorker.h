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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QReadWriteLock>
#include <QtCore/QVector>

#include "abstractVirtualSensorWorker.h"

namespace trikControl {

/// Worker object that processes color sensor output and updates stored reading. Meant to be executed in separate
/// thread.
class ColorSensorWorker : public AbstractVirtualSensorWorker
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param script - file name of a scrit used to start or stop a sensor.
	/// @param inputFile - sensor input fifo. Note that we will write data here, not read it.
	/// @param outputFile - sensor output fifo. Note that we will read sensor data from here.
	/// @param m - horisontal dimension of a sensor.
	/// @param n - vertical dimension of a sensor.
	/// @param state - shared state of a sensor.
	ColorSensorWorker(const QString &script, const QString &inputFile, const QString &outputFile, int m, int n
		, DeviceState &state, trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~ColorSensorWorker() override;

public slots:
	/// Initializes a camera.
	/// @param showOnDisplay - true if we want an image from a camera to be drawn on robot display.
	void init(bool showOnDisplay);

	/// Returns dominant color in given cell of a grid as a vector [R; G; B] in RGB color scale.
	/// If m or n are out of range, returns [-1; -1; -1].
	/// Can be accessed directly from other thread.
	QVector<int> read(int m, int n);

private:
	QString sensorName() const override;

	void onNewData(const QString &dataLine) override;

	/// Current stored reading of a sensor. First two vectors are m*n matrix, inner vector contains 3 values --- red,
	/// green and blue components of a dominant color in this cell.
	QVector<QVector<QVector<int>>> mReading;

	/// Temporary buffer for new readings of a sensor. When all data is received, it is synced with mReading.
	QVector<QVector<QVector<int>>> mReadingBuffer;

	/// True, if video stream from camera shall be shown on robot display.
	bool mShowOnDisplay = true;
};

}
