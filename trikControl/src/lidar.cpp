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

#include <QsLog.h>
#include <trikKernel/configurer.h>

#include "port/portManager.h"

using namespace trikControl;

Lidar::Lidar(const QString &port, const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction, PortManager &ports)
	: mState("Lidar on " + port)
{
	// A port listed in <ports> is a UART port: acquire it and take the device
	// file from the lease. A port that is not configured is treated as a USB
	// device (u1/u2/u3), in which case the file must come from Configurer.
	if (ports.isConfigured(port)) {
		mPortLease = ports.acquire(port, PortMode::Uart, QStringLiteral("lidar"));
		if (!mPortLease) {
			QLOG_ERROR() << "Lidar on" << port << "cannot acquire UART port";
			mState.fail();
			return;
		}
	}

	QString file;
	file = configurer.attributeByPort(port, QStringLiteral("file"), &file);
	if (file.isEmpty()) {
		file = mPortLease.device();
	}

	if (file.isEmpty()) {
		QLOG_ERROR() << "Lidar on" << port << "has no device file and no port-managed device";
		mState.fail();
		return;
	}

	mLidarWorker = new LidarWorker(file, hardwareAbstraction);
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
	return mLidarWorker ? mLidarWorker->status() : mState.status();
}

QVector<int> Lidar::read() const
{
	if (!mLidarWorker) {
		return {};
	}

	QVector<int> result;
	QMetaObject::invokeMethod(mLidarWorker, [this, &result]() { result = mLidarWorker->read(); },
		Qt::BlockingQueuedConnection);
	return result;
}

QVector<int> Lidar::readRaw() const
{
	if (!mLidarWorker) {
		return {};
	}

	QVector<int> result;
	QMetaObject::invokeMethod(mLidarWorker, [this, &result]() { result = mLidarWorker->readRaw(); },
		Qt::BlockingQueuedConnection);
	return result;
}
