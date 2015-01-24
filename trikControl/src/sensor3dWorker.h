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
#include <QtCore/QSocketNotifier>
#include <QtCore/QScopedPointer>
#include <QtCore/QVector>
#include <QtCore/QReadWriteLock>

namespace trikControl {

/// Handles events from sensor, intended to work in separate thread
class Sensor3dWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param min - minimal actual (physical) value returned by sensor. Used to normalize returned values.
	/// @param max - maximal actual (physical) value returned by sensor. Used to normalize returned values.
	/// @param deviceFile - device file for this sensor.
	Sensor3dWorker(int min, int max, QString const &deviceFile);

signals:
	/// Emitted when new sensor reading is ready.
	void newData(QVector<int> reading);

public slots:
	/// Returns current raw reading of a sensor in a form of vector with 3 coordinates.
	QVector<int> read();

private slots:
	/// Updates current reading when new value is ready.
	void readFile();

private:
	QScopedPointer<QSocketNotifier> mSocketNotifier;

	QVector<int> mReading;
	QVector<int> mReadingUnsynced;

	int mDeviceFileDescriptor;
	int mMax;
	int mMin;
	QReadWriteLock mLock;
};

}
