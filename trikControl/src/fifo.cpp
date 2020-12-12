/* Copyright 2015 CyberTech Labs Ltd.
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

#include "src/fifo.h"

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <QEventLoop>

#include "src/configurerHelper.h"

using namespace trikControl;

Fifo::Fifo(const QString &virtualPort, const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: Fifo(configurer.attributeByPort(virtualPort, "file"), hardwareAbstraction)
{
}

Fifo::Fifo(const QString &fileName, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mFifoWorker(new FifoWorker(fileName, hardwareAbstraction))
{
	mFifoWorker->moveToThread(&mWorkerThread);

	connect(mFifoWorker, &FifoWorker::newLine, this, &Fifo::newLine);
	connect(mFifoWorker, &FifoWorker::newData, this, &Fifo::newData);
	connect(&mWorkerThread, &QThread::started, mFifoWorker, &FifoWorker::init);
	connect(&mWorkerThread, &QThread::finished, mFifoWorker, &QObject::deleteLater);

	mWorkerThread.setObjectName(mFifoWorker->metaObject()->className());
	mWorkerThread.start();
	mFifoWorker->waitUntilInited();
}

Fifo::~Fifo()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

DeviceInterface::Status Fifo::status() const
{
	return mFifoWorker->status();
}

QString Fifo::read()
{
	QString result;
	if (hasLine()) {
		QMetaObject::invokeMethod(mFifoWorker, [this, &result](){result = mFifoWorker->read();}
								, Qt::BlockingQueuedConnection);
	}
	return result;
}

QVector<uint8_t> Fifo::readRaw()
{
	QVector<uint8_t> result;
	if (hasData()) {
		QMetaObject::invokeMethod(mFifoWorker, [this, &result](){result = mFifoWorker->readRaw();}
								, Qt::BlockingQueuedConnection);
	}
	return result;
}

bool Fifo::hasLine() const
{
	bool result;
	QMetaObject::invokeMethod(mFifoWorker, [this, &result](){result = mFifoWorker->hasLine();}
							, Qt::BlockingQueuedConnection);
	return result;
}

bool Fifo::hasData() const
{
	bool result;
	QMetaObject::invokeMethod(mFifoWorker, [this, &result](){result = mFifoWorker->hasData();}
							, Qt::BlockingQueuedConnection);
	return result;
}
