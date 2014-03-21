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

#include "src/cameraLineDetectorSensorWorker.h"

#include <QtCore/QDebug>

using namespace trikControl;

CameraLineDetectorSensor::CameraLineDetectorSensor(
		QString const &roverCvBinary
		, QString const &inputFile
		, QString const &outputFile)
	: mCameraLineDetectorSensorWorker(new CameraLineDetectorSensorWorker(roverCvBinary, inputFile, outputFile))
{
	qDebug() << "CameraLineDetectorSensor::CameraLineDetectorSensor";
	mCameraLineDetectorSensorWorker->moveToThread(&mWorkerThread);
	mCameraLineDetectorSensorWorker->moveChildrenToCorrectThread();
	mWorkerThread.start();
}

CameraLineDetectorSensor::~CameraLineDetectorSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

void CameraLineDetectorSensor::init()
{
	qDebug() << "CameraLineDetectorSensor::init()";
	QMetaObject::invokeMethod(mCameraLineDetectorSensorWorker.data(), "init");
}

void CameraLineDetectorSensor::detect()
{
	qDebug() << "CameraLineDetectorSensor::detect()";
	QMetaObject::invokeMethod(mCameraLineDetectorSensorWorker.data(), "detect");
}

int CameraLineDetectorSensor::read()
{
	return mCameraLineDetectorSensorWorker->read();
}
