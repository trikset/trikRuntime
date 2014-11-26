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

#include "src/abstractVirtualSensorWorker.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

using namespace trikControl;

AbstractVirtualSensorWorker::AbstractVirtualSensorWorker(QString const &script, QString const &inputFile
		, QString const &outputFile)
	: mScript(script)
	, mSensorProcess(this)
	, mInputFile(inputFile)
	, mOutputFile(outputFile)
{
}

AbstractVirtualSensorWorker::~AbstractVirtualSensorWorker()
{
	stop();
}

void AbstractVirtualSensorWorker::stop()
{
	if (mReady) {
		deinitialize();
	}
}

void AbstractVirtualSensorWorker::init()
{
	if (mReady && mInputFile.exists() && mOutputFile.exists()) {
		// Sensor is up and ready.
		return;
	} else if (!mInputFile.exists() || !mOutputFile.exists()) {
		// Sensor is down.
		startVirtualSensor();
	} else {
		// Sensor is already running but we are not connected to it.
		openFifos();
	}
}

void AbstractVirtualSensorWorker::readFile()
{
	char data[4000] = {0};

	mSocketNotifier->setEnabled(false);

	if (::read(mOutputFileDescriptor, data, 4000) < 0) {
		qDebug() << mOutputFile.fileName() << ": fifo read failed: " << errno;
		return;
	}

	mBuffer += data;

	if (mBuffer.contains("\n")) {
		QStringList lines = mBuffer.split('\n', QString::KeepEmptyParts);

		mBuffer = lines.last();
		lines.removeLast();

		for (QString const line : lines) {
			onNewData(line);
		}
	}

	mSocketNotifier->setEnabled(true);
}

bool AbstractVirtualSensorWorker::launchSensorScript(QString const &command)
{
	qDebug() << "Sending" << command << "command to" << sensorName() << "sensor";

	QFileInfo const scriptFileInfo(mScript);

	if (mSensorProcess.state() == QProcess::Running) {
		mSensorProcess.close();
	}

	QStringList const params{command};

	mSensorProcess.setWorkingDirectory(scriptFileInfo.absolutePath());
	mSensorProcess.start(scriptFileInfo.filePath(), params, QIODevice::ReadOnly | QIODevice::Unbuffered);

	mSensorProcess.waitForStarted();

	if (mSensorProcess.state() != QProcess::Running) {
		qDebug() << "Cannot launch" << sensorName() << "script" << scriptFileInfo.filePath() << " in "
				<< scriptFileInfo.absolutePath();
		return false;
	}

	if (!mSensorProcess.waitForFinished()) {
		qDebug() << sensorName() << "script" << scriptFileInfo.filePath() << " in " << scriptFileInfo.absolutePath()
				<< "hanged up or finished unexpectedly!";
		return false;
	}

	QString const processOutput = mSensorProcess.readAllStandardOutput() + mSensorProcess.readAllStandardError();
	if (processOutput.contains("error")) {
		qDebug() << sensorName() << "script reported error:" << processOutput;
		return false;
	}

	qDebug() << "Sensor process output:" << processOutput;

	return true;
}

void AbstractVirtualSensorWorker::startVirtualSensor()
{
	if (launchSensorScript("start")) {
		qDebug() << sensorName() << "sensor started, waiting for it to initialize...";
		openFifos();
	}
}

void AbstractVirtualSensorWorker::openFifos()
{
	if (mInputFile.isOpen()) {
		mInputFile.close();
	}

	qDebug() << "Opening" << mOutputFile.fileName();

	/// @todo Is it possible to use QFile (and QFile::handle()) here?
	mOutputFileDescriptor = open(mOutputFile.fileName().toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);

	if (mOutputFileDescriptor == -1) {
		qDebug() << "Cannot open" << sensorName() << "sensor output file" << mOutputFile.fileName();
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mOutputFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);

	qDebug() << "Opening" << mInputFile.fileName();

	if (!mInputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Sensor input file" << mInputFile.fileName() << " failed to open";
		return;
	}

	mInputStream.setDevice(&mInputFile);

	mReady = true;

	qDebug() << sensorName() << "initialization completed";

	sync();
}

void AbstractVirtualSensorWorker::sendCommand(QString const &command)
{
	mCommandQueue << command;
	sync();
}

void AbstractVirtualSensorWorker::deinitialize()
{
	if (mSocketNotifier) {
		disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
		mSocketNotifier->setEnabled(false);
	}

	if (::close(mOutputFileDescriptor) != 0) {
		qDebug() << mOutputFile.fileName() << ": fifo close failed: " << errno;
	}

	mOutputFileDescriptor = -1;
	mInputFile.close();

	if (!launchSensorScript("stop")) {
		qDebug() << "Failed to stop" << sensorName() << "sensor!";
	} else {
		qDebug() << "Successfully stopped" << sensorName() << "sensor";
	}

	emit stopped();

	mReady = false;
}

void AbstractVirtualSensorWorker::sync()
{
	if (mReady) {
		for (QString const &command : mCommandQueue) {
			mInputStream << command + "\n";
			mInputStream.flush();
		}

		mCommandQueue.clear();
	}
}
