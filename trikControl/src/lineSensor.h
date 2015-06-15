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
#include <QtCore/QThread>
#include <QtCore/QScopedPointer>
#include <QtCore/QVector>

#include "lineSensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class LineSensorWorker;

/// Implementation of virtual line sensor for real robot.
class LineSensor : public LineSensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	LineSensor(const QString &port, const trikKernel::Configurer &configurer);

	~LineSensor() override;

	Status status() const override;

public slots:
	void init(bool showOnDisplay) override;

	void detect() override;

	QVector<int> read() override;

	void stop() override;

	QVector<int> getDetectParameters() const override;

private slots:
	void onStopped();

private:
	/// State of the device, shared with worker.
	DeviceState mState;

	/// Worker object that handles sensor in separate thread.
	QScopedPointer<LineSensorWorker> mLineSensorWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}
