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

#include <QtCore/QThread>
#include <QtCore/QScopedPointer>

#include "objectSensorInterface.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class ObjectSensorWorker;

/// Implementation of object sensor for real robot.
class ObjectSensor : public ObjectSensorInterface
{
	Q_OBJECT

public:
	ObjectSensor(QString const &port, trikKernel::Configurer const &configurer);

	~ObjectSensor() override;

public slots:
	void init(bool showOnDisplay) override;

	void detect() override;

	QVector<int> read() override;

	void stop() override;

private:
	/// Worker object that handles sensor in separate thread.
	QScopedPointer<ObjectSensorWorker> mObjectSensorWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}
