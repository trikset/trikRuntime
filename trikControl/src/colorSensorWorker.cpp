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

#include "colorSensorWorker.h"

#include <QsLog.h>

#include "exceptions/incorrectDeviceConfigurationException.h"

using namespace trikControl;

ColorSensorWorker::ColorSensorWorker(const QString &script, const QString &inputFile, const QString &outputFile
	, int m, int n, DeviceState &state, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: AbstractVirtualSensorWorker(script, inputFile, outputFile, state, hardwareAbstraction)
{
	if (m <= 0) {
		state.fail();
		throw IncorrectDeviceConfigurationException("Color Sensor shall have 'm' parameter greater than zero");
	}

	if (n <= 0) {
		state.fail();
		throw IncorrectDeviceConfigurationException("Color Sensor shall have 'n' parameter greater than zero");
	}

	const auto init = [m, n](QVector<QVector<QVector<int>>> &reading) {
		reading.resize(m);
		for (int i = 0; i < m; ++i) {
			reading[i].resize(n);
			for (int j = 0; j < n; ++j) {
				reading[i][j] = {0, 0, 0};
			}
		}
	};

	init(mReading);
	init(mReadingBuffer);
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
		QLOG_WARN() <<
		QString("Incorrect parameters for ColorSensorWorker::read: m = %1, n = %2").arg(m).arg(n);
		return {-1, -1, -1};
	}

	return mReading[m - 1][n - 1];
}

QString ColorSensorWorker::sensorName() const
{
	return "Color sensor";
}

void ColorSensorWorker::onNewData(const QString &dataLine)
{
	QStringList const parsedLine = dataLine.split(" ", QString::SkipEmptyParts);

	if (parsedLine[0] == "color:") {

		if (parsedLine.size() <= mReadingBuffer.size() * mReadingBuffer[0].size()) {
			// Data is corrupted, for example, by other process that have read part of data from FIFO.
			QLOG_WARN() << "Corrupted data in sensor output queue:" << dataLine;
			return;
		}

		for (int i = 0; i < mReadingBuffer.size(); ++i) {
			for (int j = 0; j < mReadingBuffer[i].size(); ++j) {
				unsigned const int colorValue = parsedLine[i * mReadingBuffer.size() + j + 1].toUInt();
				const int r = (colorValue >> 16) & 0xFF;
				const int g = (colorValue >> 8) & 0xFF;
				const int b = colorValue & 0xFF;
				mReadingBuffer[i][j] = {r, g, b};
			}
		}

		mReading.swap(mReadingBuffer);
	}
}
