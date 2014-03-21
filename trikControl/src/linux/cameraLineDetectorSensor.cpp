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

#include "include/trikControl/cameraLineDetectorSensor.h"

#include <QtCore/QDebug>

#include "src/cameraLineDetectorSensorWorker.h"

using namespace trikControl;

CameraLineDetectorSensor::CameraLineDetectorSensor(
		QString const &roverCvBinary
		, QString const &inputFile
		, QString const &outputFile)
	: mCameraLineDetectorSensorWorker(new CameraLineDetectorSensorWorker(roverCvBinary, inputFile, outputFile))
{
	connect(this, SIGNAL(threadDetect()), mCameraLineDetectorSensorWorker, SLOT(detect()));
	mCameraLineDetectorSensorWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();
}

CameraLineDetectorSensor::~CameraLineDetectorSensor()
{
}

void CameraLineDetectorSensor::detect()
{
	emit threadDetect();
}

int CameraLineDetectorSensor::read()
{
	return mCameraLineDetectorSensorWorker->read();
}
