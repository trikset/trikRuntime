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

#include "src/colorSensorWorker.h"

#include <QtCore/QDebug>

using namespace trikControl;

ColorSensorWorker::ColorSensorWorker(QString const &script, QString const &inputFile, QString const &outputFile
		, int m, int n)
	: AbstractVirtualSensorWorker(script, inputFile, outputFile)
{
	/// @todo Throw an exception here.
	Q_ASSERT(m > 0);
	Q_ASSERT(n > 0);

	mReading.resize(m);
	for (int i = 0; i < m; ++i) {
		mReading[i].resize(n);
	}
}

ColorSensorWorker::~ColorSensorWorker()
{
}

void ColorSensorWorker::init(bool showOnDisplay)
{
	AbstractVirtualSensorWorker::init();
	sendCommand(QString("video_out %1").arg(showOnDisplay ? 1 : 0));
}

QVector<int> ColorSensorWorker::read(int m, int n)
{
	if(m > mReading.size() || n > mReading[0].size() || m <= 0 || n <= 0) {
		return {-1, -1, -1};
	}

	mLock.lockForRead();
	QVector<int> result = mReading[m - 1][n - 1];
	mLock.unlock();

	return result;
}

QString ColorSensorWorker::sensorName() const
{
	return "Color sensor";
}

void ColorSensorWorker::onNewData(QString const &dataLine)
{
	QStringList const parsedLine = dataLine.split(" ", QString::SkipEmptyParts);

	if (parsedLine[0] == "color:") {
		mLock.lockForWrite();
		for (int i = 0; i < mReading.size(); ++i) {
			for (int j = 0; j < mReading[i].size(); ++j) {
				unsigned int const colorValue = parsedLine[i * mReading.size() + j + 1].toUInt();
				int const r = (colorValue >> 16) & 0xFF;
				int const g = (colorValue >> 8) & 0xFF;
				int const b = colorValue & 0xFF;
				mReading[i][j] = {r, g, b};
			}
		}

		mLock.unlock();
	}
}
