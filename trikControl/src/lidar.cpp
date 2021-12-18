/* Copyright 2021 CyberTech Labs Ltd.
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

#include "lidar.h"

#include <trikKernel/configurer.h>

using namespace trikControl;

Lidar::Lidar(const QString &port, const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mLidarWorker(new LidarWorker(configurer.attributeByPort(port, "file"), hardwareAbstraction))
{
	mLidarWorker->moveToThread(&mWorkerThread);

	connect(&mWorkerThread, &QThread::started, mLidarWorker, &LidarWorker::init);
	connect(&mWorkerThread, &QThread::finished, mLidarWorker, &QObject::deleteLater);

	mWorkerThread.setObjectName(mLidarWorker->metaObject()->className());
	mWorkerThread.start();
	mLidarWorker->waitUntilInited();
}

Lidar::~Lidar()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

Lidar::Status Lidar::status() const
{
	return mLidarWorker->status();
}

QVector<int> Lidar::read() const
{
	QVector<int> result;
	QMetaObject::invokeMethod(mLidarWorker, [this, &result](){result = mLidarWorker->read();}
							, Qt::BlockingQueuedConnection);
	return result;
}

QVector<int> Lidar::readRaw() const
{
	QVector<int> result;
	QMetaObject::invokeMethod(mLidarWorker, [this, &result](){result = mLidarWorker->readRaw();}
							, Qt::BlockingQueuedConnection);
	return result;
}
