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

#include "src/abstractVirtualSensorWorker.h"

#include <QtCore/QFileInfo>

#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#include <trikHal/hardwareAbstractionInterface.h>

#include <QsLog.h>

using namespace trikControl;

AbstractVirtualSensorWorker::AbstractVirtualSensorWorker(const QString &script, const QString &inputFile
		, const QString &outputFile, DeviceState &state, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mSystemConsole(hardwareAbstraction.systemConsole())
	, mScript(script)
	, mInputFile(hardwareAbstraction.createOutputDeviceFile(inputFile))
	, mState(state)
	, mHardwareAbstraction(hardwareAbstraction)
	, mOutputFile(outputFile)
{
}

AbstractVirtualSensorWorker::~AbstractVirtualSensorWorker()
{
	if (mState.isReady()) {
		stop();
	}
}

AbstractVirtualSensorWorker::Status AbstractVirtualSensorWorker::status() const
{
	return mState.status();
}

void AbstractVirtualSensorWorker::stop()
{
	if (mState.isReady()) {
		/// @todo Correctly stop starting sensor.
		mState.stop();
		deinitialize();
	}
}

void AbstractVirtualSensorWorker::init()
{
	mOutputFifo.reset(mHardwareAbstraction.createFifo(mOutputFile));

	if (mState.isReady() && QFileInfo::exists(mInputFile->fileName()) && QFileInfo::exists(mOutputFifo->fileName())) {
		// Sensor is up and ready.
		QLOG_ERROR() << "Trying to init video sensor that is already running, ignoring";
		return;
	}

	if (mState.status() == DeviceInterface::Status::starting) {
		// Sensor is already starting.
		QLOG_ERROR() << "Trying to start a sensor that is already starting, ignoring";
		return;
	}

	mState.start();
	if (!QFileInfo::exists(mInputFile->fileName()) || !QFileInfo::exists(mOutputFifo->fileName())) {
		// Sensor is down.
		startVirtualSensor();
	} else {
		// Sensor is already running but we are not connected to it.
		openFifos();
	}

	mState.ready();
}

void AbstractVirtualSensorWorker::onNewDataInOutputFifo(const QString &data)
{
	onNewData(data);
}

bool AbstractVirtualSensorWorker::launchSensorScript(const QString &command)
{
	QLOG_INFO() << "Sending" << command << "command to" << sensorName() << "sensor";

	QString processOutput;
	if (!mSystemConsole.startProcessSynchronously(mScript, {command}, &processOutput)) {
		mState.fail();
		return false;
	}

	if (processOutput.contains("error")) {
		QLOG_ERROR() << sensorName() << "script reported error:" << processOutput;
		mState.fail();
		return false;
	}

	QLOG_INFO() << "Sensor process output:" << processOutput;

	return true;
}

void AbstractVirtualSensorWorker::startVirtualSensor()
{
	if (launchSensorScript("start")) {
		QLOG_INFO() << sensorName() << "sensor started, waiting for it to initialize...";
		openFifos();
	}
}

void AbstractVirtualSensorWorker::openFifos()
{
	mInputFile->close();

	QLOG_INFO() << "Opening" << mOutputFifo->fileName();

	connect(mOutputFifo.data(), &trikHal::FifoInterface::newLine
			, this, &AbstractVirtualSensorWorker::onNewDataInOutputFifo);

	if (!mOutputFifo->open()) {
		mState.fail();
		return;
	}


	QLOG_INFO() << "Opening" << mInputFile->fileName();

	if (!mInputFile->open()) {
		mState.fail();
		return;
	}

	QLOG_INFO() << sensorName() + " initialization completed";

	sync();
}

void AbstractVirtualSensorWorker::sendCommand(const QString &command)
{
	mCommandQueue << command;
	sync();
}

void AbstractVirtualSensorWorker::deinitialize()
{
	if (!mOutputFifo->close()) {
		mState.fail();
	}

	mInputFile->close();

	if (!launchSensorScript("stop")) {
		QLOG_ERROR() << QString("Failed to stop %1 sensor!").arg(sensorName());
		mState.fail();
	} else {
		QLOG_INFO() << QString("Successfully stopped %1 sensor").arg(sensorName());
	}

	emit stopped();

	mState.off();
}

void AbstractVirtualSensorWorker::sync()
{
	if (mState.isReady()) {
		for (const QString &command : mCommandQueue) {
			mInputFile->write(command + "\n");
		}

		mCommandQueue.clear();
	}
}
