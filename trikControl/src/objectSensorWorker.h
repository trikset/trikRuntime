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

/// Worker object that processes object sensor output and updates stored reading. Meant to be executed in separate
/// thread.
class ObjectSensorWorker : public AbstractVirtualSensorWorker
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param script - file name of a scrit used to start or stop a sensor.
	/// @param inputFile - sensor input fifo. Note that we will write data here, not read it.
	/// @param outputFile - sensor output fifo. Note that we will read sensor data from here.
	/// @param toleranceFactor - a value on which hueTolerance, saturationTolerance and valueTolerance is multiplied
	///        after "detect" command. Higher values allow to count more points on an image as tracked object.
	ObjectSensorWorker(QString const &script, QString const &inputFile, QString const &outputFile
			, qreal toleranceFactor, DeviceState &state);

	~ObjectSensorWorker() override;

public slots:
	/// Initializes a camera.
	/// @param showOnDisplay - true if we want an image from a camera to be drawn on robot display.
	void init(bool showOnDisplay);

	/// Detects the color of an object in center of current frame and memorizes it.
	void detect();

	/// Returns current raw x coordinate of detected object. Sensor returns 0 if detect() was not called.
	/// Can be accessed directly from other thread.
	QVector<int> read();

private:
	QString sensorName() const override;

	void onNewData(QString const &dataLine) override;

	/// Current stored reading of a sensor.
	QVector<int> mReading;

	/// A value on which hueTolerance, saturationTolerance and valueTolerance is multiplied after "detect" command.
	qreal mToleranceFactor = 1.0;

	/// True, if video stream from camera shall be shown on robot display.
	bool mShowOnDisplay = true;

	/// Lock for a thread to disallow reading sensor values at the same time as updating them.
	QReadWriteLock mLock;
};

}
