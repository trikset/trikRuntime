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

/// Worker object that processes rover-cv fifo and updates stored reading.
class CameraLineDetectorSensorWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param roverCvBinary - binary file of rover-cv program which is used to work with video
	/// @param inputFile - rover-cv input fifo. Note that we will write data here, not read it.
	/// @param outputFile - rover-cv output fifo. Note that we will read sensor data from here.
	/// @param toleranceFactor - a value on which hueTolerance, saturationTolerance and valueTolerance is multiplied
	///        after "detect" command. Higher values allow to count more points on an image as tracked object.
	CameraLineDetectorSensorWorker(QString const &roverCvBinary, QString const &inputFile
				, QString const &outputFile, double toleranceFactor);

	/// Destructor.
	~CameraLineDetectorSensorWorker();

public slots:
	/// Initializes a camera and begins showing image from it on display.
	void init();

	/// Detects the color of an object in center of current frame and memorizes it.
	void detect();

	/// Returns current raw x coordinate of detected object. Sensor returns 0 if detect() was not called.
	/// Can be accessed directly from other thread.
	int read();

private slots:
	/// Called when rover-cv reports error. Note that it can still die silently.
	void onRoverCvError(QProcess::ProcessError error);

	/// Called when new data is available on stdout of rover-cv. Note that process stdout is buffered, so this method
	/// can be called much later than expected.
	void onRoverCvReadyReadStandardOutput();

	/// Called when new data is available on stderr of rover-cv.
	void onRoverCvReadyReadStandardError();

	/// Updates current reading when new value is ready.
	void readFile();

private:
	/// Starts rover-cv if needed and opens its fifos.
	void initDetector();

	/// Starts rover-cv process.
	void startRoverCv();

	/// Opens input and output fifos for rover-cv.
	void openFifos();

	/// Puts queued commands to rover-cv input fifo and clears queue if rover-cv is ready, otherwise does nothing.
	/// Life without continuations is pain.
	void tryToExecute();

	/// Closes fifos. Does not kill rover-cv.
	void deinitialize();

	/// Listener for output fifo.
	QScopedPointer<QSocketNotifier> mSocketNotifier;

	/// Current stored reading of a sensor.
	int mReading;

	/// File name (with path) of rover-cv binary.
	QString mRoverCvBinary;

	/// File descriptor for output fifo.
	int mOutputFileDescriptor;

	/// rover-cv process.
	QProcess mRoverCvProcess;

	/// Input fifo.
	QFile mInputFile;

	/// Output fifo.
	QFile mOutputFile;

	/// File stream for command fifo. Despite its name it is used to output commands. It is input for rover-cv.
	QTextStream mInputStream;

	/// Flag that sensor is ready and waiting for commands.
	bool mReady;

	/// A queue of commands to be passed to input fifo when it is ready.
	QList<QString> mCommandQueue;

	/// A value on which hueTolerance, saturationTolerance and valueTolerance is multiplied after "detect" command.
	double mToleranceFactor;
};

}
