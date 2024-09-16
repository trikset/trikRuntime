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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QReadWriteLock>
#include <QtCore/QVector>

#include "abstractVirtualSensorWorker.h"

namespace trikControl {

/// Worker object that processes sound sensor output and updates stored reading.
/// Meant to be executed in separate thread.
class SoundSensorWorker : public AbstractVirtualSensorWorker
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param script - file name of a scrit used to start or stop a sensor.
	/// @param inputFile - sensor input fifo. Note that we will write data here, not read it.
	/// @param outputFile - sensor output fifo. Note that we will read sensor data from here.
	SoundSensorWorker(const QString &script, const QString &inputFile, const QString &outputFile, DeviceState &state
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~SoundSensorWorker() override;

public slots:
	/// Initializes an ALSA.
	/// @param showOnDisplay - true if we want an image from a DSP to be drawn on robot display.
	void init(bool showOnDisplay);

	/// Not use.
	/// @todo ???
	void detect();

	/// Sets volume coefficient
	void volume(int volCoeff);

	/// Returns angle and volume for both channels. Can be accessed directly from other thread.
	QVector<int> read();

private:
	QString sensorName() const override;

	void onNewData(QString const &dataLine) override;

	/// Current stored reading of a sensor.
	QVector<int> mReading;

	/// True, if video stream from DSP shall be shown on robot display.
	bool mShowOnDisplay = true;

	/// Lock for a thread to disallow reading sensor values at the same time as updating them.
	QReadWriteLock mLock;
};

}
