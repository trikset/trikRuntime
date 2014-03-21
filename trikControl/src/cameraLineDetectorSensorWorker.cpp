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

#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <linux/input.h>

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
}

CameraLineDetectorSensorWorker::~CameraLineDetectorSensorWorker()
{
	disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFifo()));
	mSocketNotifier->setEnabled(false);
	if (::close(mOutputFileDescriptor) != 0) {
		qDebug() << mOutputFile.fileName() << ": fifo close failed: " << errno;
	}
}

void CameraLineDetectorSensorWorker::detect()
{
	qDebug() << "Detect was called";

	if (!mReady) {
		initDetector();
	}

	if (!mReady) {
		return;
	}

	qDebug() << "detect";

	mInputStream << "detect\n";
	mInputStream.flush();
}

void CameraLineDetectorSensorWorker::initDetector()
{
	qDebug() << "initDetector()";

	if (!mOutputFile.exists()) {
		QFileInfo roverCvBinaryFileInfo(mRoverCvBinary);

		qDebug() << "Starting rover-cv";

		mRoverCvProcess.setWorkingDirectory(roverCvBinaryFileInfo.absolutePath());
		mRoverCvProcess.start(roverCvBinaryFileInfo.filePath());

		mRoverCvProcess.waitForStarted();

		qDebug() << "rover-cv started";

		if (mRoverCvProcess.state() != QProcess::Running)
		{
			qDebug() << "Cannot launch detector application " << roverCvBinaryFileInfo.filePath() << " in "
					<< roverCvBinaryFileInfo.absolutePath();
			return;
		}

		// Give rover-cv time to initialize.
		/// @todo Implement correct reading from process output and parsing of "Entering video thread loop" message.
		QWaitCondition waitCondition;
		QMutex mutex;
		waitCondition.wait(&mutex, 1000);

		qDebug() << "rover-cv initialization timeout";
	}

	qDebug() << "opening" << mOutputFile.fileName();

	mOutputFileDescriptor = open(mOutputFile.fileName().toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);
	if (mOutputFileDescriptor == -1) {
		qDebug() << "Cannot open sensor output file " << mOutputFile.fileName();
		return;
	}

	qDebug() << mOutputFileDescriptor;

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

	qDebug() << mSocketNotifier->isEnabled();
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

int CameraLineDetectorSensorWorker::read()
{
	return mReading;
}
