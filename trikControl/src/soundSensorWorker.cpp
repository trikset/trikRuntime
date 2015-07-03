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

#include "soundSensorWorker.h"

#include <QtCore/QDebug>

using namespace trikControl;

SoundSensorWorker::SoundSensorWorker(QString const &script, QString const &inputFile, QString const &outputFile, DeviceState &state)
	: AbstractVirtualSensorWorker(script, inputFile, outputFile, state)
{
}


SoundSensorWorker::~SoundSensorWorker()
{
}

void SoundSensorWorker::init(bool showOnDisplay)
{
	AbstractVirtualSensorWorker::init();
	sendCommand(QString("video_out %1").arg(showOnDisplay ? 1 : 0));
}

void SoundSensorWorker::detect()
{
	sendCommand("detect");
}

QVector<int> SoundSensorWorker::read()
{
	mLock.lockForRead();
	QVector<int> result = mReading;
	mLock.unlock();

	return result;
}

QString SoundSensorWorker::sensorName() const
{
	return "Sound sensor";
}

void SoundSensorWorker::onNewData(QString const &dataLine)
{
	QStringList const parsedLine = dataLine.split(" ", QString::SkipEmptyParts);

	if (parsedLine[0] == "sound:") {
		int const angle = parsedLine[1].toInt();
		int const lvolume = parsedLine[2].toInt();
		int const rvolume = parsedLine[3].toInt();

		mLock.lockForWrite();
		mReading = {angle, lvolume, rvolume};
		mLock.unlock();
	}

}
