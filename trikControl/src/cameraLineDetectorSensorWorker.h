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
#include <QtCore/QScopedPointer>
#include <QtCore/QSocketNotifier>
#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QList>

#include "declSpec.h"
#include "sensor.h"

namespace trikControl {

class TRIKCONTROL_EXPORT CameraLineDetectorSensorWorker : public Sensor
{
	Q_OBJECT

public:
	CameraLineDetectorSensorWorker(QString const &roverCvBinary, QString const &inputFile, QString const &outputFile);
	~CameraLineDetectorSensorWorker();
	void moveChildrenToCorrectThread();

public slots:
	/// Initializes a camera and begins showing image from it on display.
	void init();

	/// Detects the color of an object in center of current frame and memorizes it.
	void detect();

	/// Returns current raw x coordinate of detected object. Sensor returns 0 if detect() was not called.
	/// Can be accessed directly from other thread.
	int read();

private slots:
	void onRoverCvError(QProcess::ProcessError error);
	void onRoverCvReadyReadStandardOutput();
	void onRoverCvReadyReadStandardError();

	/// Updates current reading when new value is ready.
	void readFile();

private:
	void initDetector();
	void startRoverCv();
	void openFifos();
	void tryToExecute();
	void deinitialize();

	QScopedPointer<QSocketNotifier> mSocketNotifier;
	int mReading;
	QString mRoverCvBinary;
	int mOutputFileDescriptor;
	QProcess mRoverCvProcess;
	QFile mInputFile;
	QFile mOutputFile;
	QTextStream mInputStream;
	bool mReady;
	QList<QString> mCommandQueue;
};

}
