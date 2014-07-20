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
	: AbstractVirtualSensorWorker(script, inputFile, outputFile)
	, mToleranceFactor(toleranceFactor)
{
}

LineSensorWorker::~LineSensorWorker()
{
}

void LineSensorWorker::init(bool showOnDisplay)
{
	AbstractVirtualSensorWorker::init();
	sendCommand(QString("video_out %1").arg(showOnDisplay ? 1 : 0));
}

void LineSensorWorker::detect()
{
	sendCommand("detect");
}

int LineSensorWorker::read()
{
	return mReading;
}

QString LineSensorWorker::sensorName() const
{
	return "Line sensor";
}

void LineSensorWorker::onNewData(QString const &dataLine)
{
	QStringList const parsedLine = dataLine.split(" ", QString::SkipEmptyParts);

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
}
