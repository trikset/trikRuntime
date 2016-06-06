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

void ColorSensorWorker::init(bool showOnDisplay, bool returnHSV)
{
	AbstractVirtualSensorWorker::init();
	sendCommand(QString("video_out %1").arg(showOnDisplay ? 1 : 0));
	if (returnHSV)
	{
	        sendCommand("hsv");
	}
	mReturnHSV = returnHSV;
}

QVector<int> ColorSensorWorker::read(int m, int n)
{
	if(m > mReading.size() || n > mReading[0].size() || m <= 0 || n <= 0) {
		QLOG_WARN() << QString("Incorrect parameters for ColorSensorWorker::read: m = %1, n = %2").arg(m).arg(n);
		return {-1, -1, -1};
	}

	return hsvToRgb(mReading[m - 1][n - 1]);
}

QVector<int> ColorSensorWorker::hsvToRgb(QVector<int> hsv)
{
	int H = hsv[0];
	int S = hsv[1];
	int V = hsv[2];

	QVector<int> resultRGB({0, 0, 0});

	float r = 0;
	float g = 0;
	float b = 0;

	float h = H / 255.0f;
	float s = S / 255.0f;
	float v = V / 255.0f;

	v = v < 0.2 ? 0 : v;
	s = s < 0.2 ? 0 : 1;

	int i = h*6;
	float f = h*6-i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	double t = v * (1 - (1 - f) * s);

	switch(i % 6) {
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
	}

	int ri = r*255;
	int gi = g*255;
	int bi = b*255;

	resultRGB = {ri, gi, bi};

	return resultRGB;
}

int ColorSensorWorker::getColor(int m, int n)
{
	if (!mReturnHSV) return -1;

	int H = mReading[m - 1][n - 1][0] * 360 / 255;
	int S = mReading[m - 1][n - 1][1] * 100 / 255;
	int V = mReading[m - 1][n - 1][2] * 100 / 255;

	int segment = 360 / (COUNT_COLORS - 2);

	if (V > 90 && S < 10) return 7; //return white
	else if ((V < 15) || (V < 30 && S < 20)) return 0; //return black

	return (((H + segment / 2) / segment) + 1);
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
				const int C1 = (colorValue >> 16) & 0xFF;
				const int C2 = (colorValue >> 8) & 0xFF;
				const int C3 = colorValue & 0xFF;
 
				mReadingBuffer[i][j] = {C1, C2, C3};
			}
		}

		mReading.swap(mReadingBuffer);
	}
}
