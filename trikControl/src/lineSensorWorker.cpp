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

#include "lineSensorWorker.h"

using namespace trikControl;

LineSensorWorker::LineSensorWorker(const QString &script, const QString &inputFile, const QString &outputFile
	, qreal toleranceFactor, DeviceState &state, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: AbstractVirtualSensorWorker(script, inputFile, outputFile, state, hardwareAbstraction)
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

QVector<int> LineSensorWorker::read()
{
	QReadLocker locker(&mReadingLock);
	return mReading;
}

QVector<int> LineSensorWorker::getDetectParameters() const
{
	QReadLocker locker(&mDetectParametersLock);
	return mDetectParameters;
}

QString LineSensorWorker::sensorName() const
{
	return "Line sensor";
}

void LineSensorWorker::onNewData(const QString &dataLine)
{
	QStringList const parsedLine = dataLine.split(" ", QString::SkipEmptyParts);

	if (parsedLine[0] == "loc:") {
		const int x = parsedLine[1].toInt();
		const int crossroadsProbability = parsedLine[2].toInt();
		const int mass = parsedLine[3].toInt();

		mReadingBuffer = {x, crossroadsProbability, mass};

		QWriteLocker locker(&mReadingLock);
		mReading.swap(mReadingBuffer);
	}

	if (parsedLine[0] == "hsv:") {
		const int hue = parsedLine[1].toInt();
		const int hueTolerance = parsedLine[2].toInt();
		const int saturation = parsedLine[3].toInt();
		const int saturationTolerance = parsedLine[4].toInt();
		const int value = parsedLine[5].toInt();
		const int valueTolerance = parsedLine[6].toInt();

		const QString command = QString("hsv %0 %1 %2 %3 %4 %5 %6\n")
		                        .arg(hue)
		                        .arg(static_cast<int>(hueTolerance * mToleranceFactor))
		                        .arg(saturation)
		                        .arg(static_cast<int>(saturationTolerance * mToleranceFactor))
		                        .arg(value)
		                        .arg(static_cast<int>(valueTolerance * mToleranceFactor))
		;

		sendCommand(command);

		mDetectParametersBuffer = {hue, saturation, value, hueTolerance, saturationTolerance, valueTolerance};

		// Atomic operation, so it will prevent data corruption if value is read by another thread at the same time as
		// this thread prepares data.
		QWriteLocker locker(&mDetectParametersLock);
		mDetectParameters.swap(mDetectParametersBuffer);
	}
}
