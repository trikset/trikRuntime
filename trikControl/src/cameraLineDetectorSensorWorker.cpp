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

#include "src/cameraLineDetectorSensorWorker.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

using namespace trikControl;

CameraLineDetectorSensorWorker::CameraLineDetectorSensorWorker(
		QString const &roverCvBinary
		, QString const &inputFile
		, QString const &outputFile)
	: mReading(0)
	, mRoverCvBinary(roverCvBinary)
	, mOutputFileDescriptor(0)
	, mInputFile(inputFile)
	, mOutputFile(outputFile)
	, mReady(false)
{
	qRegisterMetaType<QProcess::ProcessError>("QProcess::ProcessError");

	connect(&mRoverCvProcess, SIGNAL(error(QProcess::ProcessError))
			, this, SLOT(onRoverCvError(QProcess::ProcessError)), Qt::QueuedConnection);

	connect(&mRoverCvProcess, SIGNAL(readyReadStandardError())
			, this, SLOT(onRoverCvReadyReadStandardError()), Qt::QueuedConnection);

	connect(&mRoverCvProcess, SIGNAL(readyReadStandardOutput())
			, this, SLOT(onRoverCvReadyReadStandardOutput()), Qt::QueuedConnection);
}

CameraLineDetectorSensorWorker::~CameraLineDetectorSensorWorker()
{
	deinitialize();
}

void CameraLineDetectorSensorWorker::moveChildrenToCorrectThread()
{
	mRoverCvProcess.moveToThread(this->thread());
}

void CameraLineDetectorSensorWorker::init()
{
	if (!mReady || !mInputFile.exists() || !mOutputFile.exists()) {
		initDetector();
	}
}

void CameraLineDetectorSensorWorker::detect()
{
	if (!mReady || !mInputFile.exists() || !mOutputFile.exists()) {
		init();
	}

	mCommandQueue << "detect";
	tryToExecute();
}

int CameraLineDetectorSensorWorker::read()
{
	return mReading;
}

void CameraLineDetectorSensorWorker::initDetector()
{
	if (!mInputFile.exists() || !mOutputFile.exists()) {
		startRoverCv();
	} else {
		openFifos();
	}
}

void CameraLineDetectorSensorWorker::onRoverCvError(QProcess::ProcessError error)
{
	qDebug() << "rover-cv error: " << error;

	mReady = false;
	deinitialize();
}

void CameraLineDetectorSensorWorker::onRoverCvReadyReadStandardOutput()
{
	QString const data = mRoverCvProcess.readAllStandardOutput();
	QStringList const lines = data.split("\n");
	foreach (QString const line, lines) {
		qDebug() << "From rover-cv:" << line;
		if (line == "Entering video thread loop") {
			openFifos();
		}
		if (line == "Terminating") {
			mReady = false;
			deinitialize();
		}
	}
}

void CameraLineDetectorSensorWorker::onRoverCvReadyReadStandardError()
{
	QString const data = mRoverCvProcess.readAllStandardError();
	QStringList const lines = data.split("\n");
	foreach (QString const line, lines) {
		qDebug() << "From rover-cv standard error:" << line;
	}
}

void CameraLineDetectorSensorWorker::readFile()
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

	foreach (QString const line, lines) {
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

			// These values are not needed in current implementation, but are left here for reference too.
			Q_UNUSED(hue)
			Q_UNUSED(hueTolerance)
			Q_UNUSED(saturation)
			Q_UNUSED(saturationTolerance)
			Q_UNUSED(value)
			Q_UNUSED(valueTolerance)

			mInputStream << QString(line).remove(":") << "\n";
			mInputStream.flush();
		}
	}

	mSocketNotifier->setEnabled(true);
}

void CameraLineDetectorSensorWorker::startRoverCv()
{
	QFileInfo roverCvBinaryFileInfo(mRoverCvBinary);

	qDebug() << "Starting rover-cv";

	if (mRoverCvProcess.state() == QProcess::Running) {
		mRoverCvProcess.close();
	}

	mRoverCvProcess.setWorkingDirectory(roverCvBinaryFileInfo.absolutePath());
	mRoverCvProcess.start(roverCvBinaryFileInfo.filePath(), QIODevice::ReadOnly | QIODevice::Unbuffered);

	mRoverCvProcess.waitForStarted();

	if (mRoverCvProcess.state() != QProcess::Running)
	{
		qDebug() << "Cannot launch detector application " << roverCvBinaryFileInfo.filePath() << " in "
				<< roverCvBinaryFileInfo.absolutePath();
		return;
	}

	qDebug() << "rover-cv started, waiting for it to initialize...";

	/// @todo Remove this hack! QProcess does not deliver messages from rover-cv during startup.
	QMutex mutex;
	QWaitCondition wait;
	wait.wait(&mutex, 1000);

	openFifos();
}

void CameraLineDetectorSensorWorker::openFifos()
{
	qDebug() << "opening" << mOutputFile.fileName();

	if (!mOutputFileDescriptor) {
		mOutputFileDescriptor = open(mOutputFile.fileName().toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);
	}

	if (mOutputFileDescriptor == -1) {
		qDebug() << "Cannot open sensor output file " << mOutputFile.fileName();
		return;
	}

	qDebug() << mOutputFileDescriptor;

	mSocketNotifier.reset(new QSocketNotifier(mOutputFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(false);

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

void CameraLineDetectorSensorWorker::tryToExecute()
{
	if (mReady) {
		foreach (QString const &command, mCommandQueue) {
			mInputStream << command + "\n";
			mInputStream.flush();
		}

		mCommandQueue.clear();
	}
}

void CameraLineDetectorSensorWorker::deinitialize()
{
	disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(false);
	if (::close(mOutputFileDescriptor) != 0) {
		qDebug() << mOutputFile.fileName() << ": fifo close failed: " << errno;
	}
}
