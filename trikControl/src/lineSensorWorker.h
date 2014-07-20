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

namespace trikControl {

/// Worker object that processes virtual sensor fifo and updates stored reading. Meant to be executed in separate
/// thread.
class LineSensorWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param script - file name of a scrit used to start or stop a sensor.
	/// @param inputFile - sensor input fifo. Note that we will write data here, not read it.
	/// @param outputFile - sensor output fifo. Note that we will read sensor data from here.
	/// @param toleranceFactor - a value on which hueTolerance, saturationTolerance and valueTolerance is multiplied
	///        after "detect" command. Higher values allow to count more points on an image as tracked object.
	LineSensorWorker(QString const &script, QString const &inputFile, QString const &outputFile
			, double toleranceFactor);

	/// Destructor.
	~LineSensorWorker();

public slots:
	/// Initializes a camera.
	/// @param showOnDisplay - true if we want an image from a camera to be drawn on robot display.
	void init(bool showOnDisplay);

	/// Detects the color of an object in center of current frame and memorizes it.
	void detect();

	/// Returns current raw x coordinate of detected object. Sensor returns 0 if detect() was not called.
	/// Can be accessed directly from other thread.
	int read();

private slots:
	/// Updates current reading when new value is ready.
	void readFile();

private:
	/// Starts virtual sensor if needed and opens its fifos.
	void initVirtualSensor();

	/// Starts virtual sensor process.
	void startVirtualSensor();

	/// Opens input and output fifos of a sensor.
	void openFifos();

	/// Puts queued commands to an input fifo and clears queue if sensor is ready, otherwise does nothing.
	void tryToExecute();

	/// Closes fifos and stops sensor.
	void deinitialize();

	/// Listener for output fifo.
	QScopedPointer<QSocketNotifier> mSocketNotifier;

	/// Current stored reading of a sensor.
	int mReading = 0;

	/// File name (with path) of a script that launches or stops sensor.
	QString mScript;

	/// File descriptor for output fifo.
	int mOutputFileDescriptor = -1;

	/// Virtual sensor process.
	QProcess mSensorProcess;

	/// Input fifo.
	QFile mInputFile;

	/// Output fifo.
	QFile mOutputFile;

	/// File stream for command fifo. Despite its name it is used to output commands. It is input for virtual sensor.
	QTextStream mInputStream;

	/// Flag that sensor is ready and waiting for commands.
	bool mReady = false;

	/// A queue of commands to be passed to input fifo when it is ready.
	QList<QString> mCommandQueue;

	/// A value on which hueTolerance, saturationTolerance and valueTolerance is multiplied after "detect" command.
	double mToleranceFactor = 1.0;

	/// True, if video stream from camera shall be shown on robot display.
	bool mShowOnDisplay = true;
};

}
