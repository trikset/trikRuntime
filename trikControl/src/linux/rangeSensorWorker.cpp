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

#include "src/rangeSensorWorker.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <QsLog.h>

using namespace trikControl;

RangeSensorWorker::RangeSensorWorker(QString const &eventFile)
	: mEventFile(eventFile)
{
}

RangeSensorWorker::~RangeSensorWorker()
{
	stop();
}

void RangeSensorWorker::stop()
{
	if (mSocketNotifier) {
		disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
		mSocketNotifier->setEnabled(false);
	}

	if (::close(mEventFileDescriptor) != 0) {
		QString const message = QString("%1: close failed: %2").arg(mEventFile.fileName()).arg(errno);
		QLOG_ERROR() << message;
		qDebug() << message;
	}

	mEventFileDescriptor = -1;
}

void RangeSensorWorker::init()
{
	QLOG_INFO() << "Opening" << mEventFile.fileName();
	qDebug() << "Opening" << mEventFile.fileName();

	/// @todo Is it possible to use QFile (and QFile::handle()) here?
	mEventFileDescriptor = open(mEventFile.fileName().toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);

	if (mEventFileDescriptor == -1) {
		QLOG_ERROR() << "Cannot open range sensor output file" << mEventFile.fileName();
		qDebug() << "Cannot open range sensor output file" << mEventFile.fileName();
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mEventFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);
}

void RangeSensorWorker::readFile()
{
	char data[4000] = {0};

	mSocketNotifier->setEnabled(false);

	if (::read(mEventFileDescriptor, data, 4000) < 0) {
		QLOG_ERROR() << mEventFile.fileName() << ": fifo read failed: " << errno;
		qDebug() << mEventFile.fileName() << ": fifo read failed: " << errno;
		return;
	}

	mBuffer += data;

	if (mBuffer.contains("\n")) {
		QStringList lines = mBuffer.split('\n', QString::KeepEmptyParts);

		mBuffer = lines.last();
		lines.removeLast();

		for (QString const &line : lines) {
			onNewData(line);
		}
	}

	mSocketNotifier->setEnabled(true);
}

void RangeSensorWorker::onNewData(QString const &dataLine)
{
	qDebug() << dataLine;
/*
	QStringList const parsedLine = dataLine.split(" ", QString::SkipEmptyParts);

	if (parsedLine[0] == "loc:") {
		int const x = parsedLine[1].toInt();
		int const crossroadsProbability = parsedLine[2].toInt();
		int const mass = parsedLine[3].toInt();

		mLock.lockForWrite();
		mReading[0] = x;
		mReading[1] = crossroadsProbability;
		mReading[2] = mass;
		mLock.unlock();
	}

	if (parsedLine[0] == "hsv:") {
		int const hue = parsedLine[1].toInt();
		int const hueTolerance = parsedLine[2].toInt();
		int const saturation = parsedLine[3].toInt();
		int const saturationTolerance = parsedLine[4].toInt();
		int const value = parsedLine[5].toInt();
		int const valueTolerance = parsedLine[6].toInt();

		QString const command = QString("hsv %0 %1 %2 %3 %4 %5 %6\n")
				.arg(hue)
				.arg(static_cast<int>(hueTolerance * mToleranceFactor))
				.arg(saturation)
				.arg(static_cast<int>(saturationTolerance * mToleranceFactor))
				.arg(value)
				.arg(static_cast<int>(valueTolerance * mToleranceFactor))
				;

		sendCommand(command);
	}
	*/
}

int RangeSensorWorker::read()
{
	return 0;
}

int RangeSensorWorker::readRawData()
{
	return 0;
}
