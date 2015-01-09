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
#include <QtCore/QThread>
#include <QtCore/QScopedPointer>
#include <QtCore/QVector>

#include "lineSensorInterface.h"

namespace trikControl {

class LineSensorWorker;

/// Implementation of virtual line sensor for real robot.
class TRIKCONTROL_EXPORT LineSensor : public LineSensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param script - file name of a scrit used to start or stop a sensor.
	/// @param inputFile - sensor input fifo. Note that we will write data here, not read it.
	/// @param outputFile - sensor output fifo. Note that we will read sensor data from here.
	/// @param toleranceFactor - a value on which hueTolerance, saturationTolerance and valueTolerance is multiplied
	///        after "detect" command. Higher values allow to count more points on an image as tracked object.
	LineSensor(QString const &script, QString const &inputFile, QString const &outputFile, double toleranceFactor);

	~LineSensor() override;

public slots:
	void init(bool showOnDisplay) override;

	void detect() override;

	QVector<int> read() override;

	void stop() override;

private:
	/// Worker object that handles sensor in separate thread.
	QScopedPointer<LineSensorWorker> mLineSensorWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}
