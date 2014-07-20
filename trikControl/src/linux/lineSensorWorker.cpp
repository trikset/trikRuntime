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

#include "src/lineSensorWorker.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

using namespace trikControl;

LineSensorWorker::LineSensorWorker(QString const &script, QString const &inputFile, QString const &outputFile
		, double toleranceFactor)
	: mScript(script)
	, mSensorProcess(this)
	, mInputFile(inputFile)
	, mOutputFile(outputFile)
	, mToleranceFactor(toleranceFactor)
	, mShowOnDisplay(true)
{
}

LineSensorWorker::~LineSensorWorker()
{
	deinitialize();
}

void LineSensorWorker::init(bool showOnDisplay)
{
	if (!mReady || !mInputFile.exists() || !mOutputFile.exists()) {
		initVirtualSensor();
		mCommandQueue << QString("video_out %1").arg(showOnDisplay ? 1 : 0);
		mShowOnDisplay = showOnDisplay;
		tryToExecute();
	}
}

void LineSensorWorker::detect()
{
	if (!mReady || !mInputFile.exists() || !mOutputFile.exists()) {
		init(mShowOnDisplay);
	}

	mCommandQueue << "detect";
	tryToExecute();
}

int LineSensorWorker::read()
{
	return mReading;
}

void LineSensorWorker::initVirtualSensor()
{
	if (!mInputFile.exists() || !mOutputFile.exists()) {
		startVirtualSensor();
	} else {
		// Sensor is already running.
		openFifos();
	}
}

void LineSensorWorker::readFile()
{
	char data[4000] = {0};
	int size = 0;

	mSocketNotifier->setEnabled(false);

	if ((size = ::read(mOutputFileDescriptor, data, 4000)) < 0) {
		qDebug() << mOutputFile.fileName() << ": fifo read failed: " << errno;
		return;
	}

	QString const linesRead(data);
	QStringList const lines = linesRead.split('\n', QString::SkipEmptyParts);

	for (QString const line : lines) {
		QStringList const parsedLine = line.split(" ", QString::SkipEmptyParts);

		qDebug() << "parsed: " << parsedLine;

		if (parsedLine[0] == "loc:") {
			int const x = parsedLine[1].toInt();
			int const angle = parsedLine[2].toInt();
			int const mass = parsedLine[3].toInt();

			mReading = x;

			// These values are not needed in current implementation, but are left here for reference.
			Q_UNUSED(angle)
			Q_UNUSED(mass)
		}

		if (parsedLine[0] == "hsv:") {
			int const hue = parsedLine[1].toInt();
			int const hueTolerance = parsedLine[2].toInt();
			int const saturation = parsedLine[3].toInt();
			int const saturationTolerance = parsedLine[4].toInt();
			int const value = parsedLine[5].toInt();
			int const valueTolerance = parsedLine[6].toInt();

			mInputStream
					<< QString("hsv %0 %1 %2 %3 %4 %5 %6")
							.arg(hue)
							.arg(hueTolerance * mToleranceFactor)
							.arg(saturation)
							.arg(saturationTolerance * mToleranceFactor)
							.arg(value)
							.arg(valueTolerance * mToleranceFactor)
					<< "\n";

			mInputStream.flush();
		}
	}

	mSocketNotifier->setEnabled(true);
}

void LineSensorWorker::startVirtualSensor()
{
	QFileInfo scriptFileInfo(mScript);

	qDebug() << "Starting line sensor";

	if (mSensorProcess.state() == QProcess::Running) {
		mSensorProcess.close();
	}

	QStringList params;
	params << "start";

	mSensorProcess.setWorkingDirectory(scriptFileInfo.absolutePath());
	mSensorProcess.start(scriptFileInfo.filePath(), params, QIODevice::ReadOnly | QIODevice::Unbuffered);

	mSensorProcess.waitForStarted();

	if (mSensorProcess.state() != QProcess::Running) {
		qDebug() << "Cannot launch detector application " << scriptFileInfo.filePath() << " in "
				<< scriptFileInfo.absolutePath();
		return;
	}

	qDebug() << "line sensor started, waiting for it to initialize...";

	/// @todo Remove this hack!
	QMutex mutex;
	QWaitCondition wait;
	wait.wait(&mutex, 1000);

	openFifos();
}

void LineSensorWorker::openFifos()
{
	qDebug() << "opening" << mOutputFile.fileName();

	if (mInputFile.isOpen()) {
		mInputFile.close();
	}

	mOutputFileDescriptor = open(mOutputFile.fileName().toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);

	if (mOutputFileDescriptor == -1) {
		qDebug() << "Cannot open line sensor output file " << mOutputFile.fileName();
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mOutputFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);

	qDebug() << "opening" << mInputFile.fileName();

	if (!mInputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Sensor input file" << mInputFile.fileName() << " failed to open";
		return;
	}

	mInputStream.setDevice(&mInputFile);

	mReady = true;

	qDebug() << "initialization completed";

	tryToExecute();
}

void LineSensorWorker::tryToExecute()
{
	if (mReady) {
		for (QString const &command : mCommandQueue) {
			mInputStream << command + "\n";
			mInputStream.flush();
		}

		mCommandQueue.clear();
	}
}

void LineSensorWorker::deinitialize()
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
}
