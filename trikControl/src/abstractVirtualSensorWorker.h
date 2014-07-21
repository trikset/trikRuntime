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

/// Base class for all virtual sensor workers. Virtual sensor is an external process that communicates using input and
/// output FIFOs and uses script that allows to start, stop or restart it. This class is a worker that is intended to
/// run in separate process and is responsible for technical side of communication with virtual server. Actual
/// protocol and interpretation of data must be implemented in descendants.
class AbstractVirtualSensorWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param script - file name of a scrit used to start or stop a sensor.
	/// @param inputFile - sensor input fifo. Note that we will write data here, not read it.
	/// @param outputFile - sensor output fifo. Note that we will read sensor data from here.
	AbstractVirtualSensorWorker(QString const &script, QString const &inputFile, QString const &outputFile);

	~AbstractVirtualSensorWorker() override;

protected:
	/// Launch sensor.
	void init();

	/// If sensor is ready, sends a command to its input FIFO, otherwise queues this command and sends it later.
	void sendCommand(QString const &command);

private slots:
	/// Updates current reading when new value is ready.
	void readFile();

private:
	/// Provides user-friendly name of a sensor used in debug output.
	virtual QString sensorName() const = 0;

	/// Called when new data is available in sensor output fifo, called separately for each line.
	virtual void onNewData(QString const &dataLine) = 0;

	/// Starts virtual sensor if needed and opens its fifos.
	void initVirtualSensor();

	/// Launches sensor control script with given command as a parameter.
	/// @returns true if sensor script launched successfully.
	bool launchSensorScript(QString const &command);

	/// Starts virtual sensor process.
	void startVirtualSensor();

	/// Opens input and output fifos of a sensor.
	void openFifos();

	/// Closes fifos and stops sensor.
	void deinitialize();

	/// Flushes queued commands to a sensor, if it is ready, otherwise does nothing.
	void sync();

	/// Listener for output fifo.
	QScopedPointer<QSocketNotifier> mSocketNotifier;

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
};

}
