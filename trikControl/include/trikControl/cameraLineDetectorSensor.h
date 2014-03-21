/* Copyright 2014 CyberTech Labs Ltd.
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

#include "declSpec.h"
#include "sensor.h"

namespace trikControl {

class CameraLineDetectorSensorWorker;

/// Uses rover-cv application to detect x coordinate of a center of an object that was in camera's field of view
/// when "detect" method was called. Used mainly to follow the line.
class TRIKCONTROL_EXPORT CameraLineDetectorSensor : public Sensor
{
	Q_OBJECT

public:
	CameraLineDetectorSensor(QString const &roverCvBinary, QString const &inputFile, QString const &outputFile);
	~CameraLineDetectorSensor();

public slots:
	/// Initializes a camera and begins showing image from it on display.
	void init();

	/// Detects the color of an object in center of current frame and memorizes it.
	void detect();

	/// Returns current raw x coordinate of detected object. Sensor returns 0 if detect() was not called.
	int read();

private:
	QScopedPointer<CameraLineDetectorSensorWorker> mCameraLineDetectorSensorWorker;

	QThread mWorkerThread;
};

}
