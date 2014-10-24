/* Copyright 2013-2014 Matvey Bryksin, Yurii Litvinov, CyberTech Labs Ltd.
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

#include "sensor3d.h"

#include "src/sensor3dWorker.h"

using namespace trikControl;

Sensor3d::Sensor3d(int min, int max, const QString &controlFile)
	: mSensor3dWorker(new Sensor3dWorker(min, max, controlFile))
{
	connect(mSensor3dWorker.data(), SIGNAL(newData(QVector<int>)), this, SIGNAL(newData(QVector<int>)));
	mSensor3dWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();
}


Sensor3d::~Sensor3d()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

QVector<int> Sensor3d::read() const
{
	return mSensor3dWorker->read();
}
