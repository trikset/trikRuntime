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

#include <QtCore/QEventLoop>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include <QsLog.h>

#include "src/configurerHelper.h"

using namespace trikControl;

Fifo::Fifo(const QString &virtualPort, const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: Fifo(configurer.attributeByPort(virtualPort, "file"), hardwareAbstraction)
{
}

Fifo::Fifo(const QString &fileName, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mFifo(hardwareAbstraction.createFifo(fileName))
	, mState("Fifo on '" + fileName + "'")
{
	mState.start();

	connect(mFifo.data(), &trikHal::FifoInterface::newData, this, &Fifo::onNewData);
	connect(mFifo.data(), &trikHal::FifoInterface::readError, this, &Fifo::onReadError);

	if (mFifo->open()) {
		mState.ready();
	} else {
		mState.fail();
	}
}

Fifo::~Fifo()
{
	if (mState.isReady()) {
		mFifo->close();
	}
}

DeviceInterface::Status Fifo::status() const
{
	return mState.status();
}

QString Fifo::read()
{
	QReadLocker r(&mCurrentLock);
	if (mCurrent.isEmpty()) {
		r.unlock();
		QEventLoop l;
		connect(this, &Fifo::newData, &l, [&l](const QString &newData) { if (!newData.isEmpty()) l.quit(); } );
		l.exec();
	}
	r.unlock();
	QString result;
	QWriteLocker w(&mCurrentLock);
	result.swap(mCurrent);
	return result;
}

bool Fifo::hasData() const
{
	QReadLocker r(&mCurrentLock);
	return !mCurrent.isEmpty();
}

void Fifo::onNewData(const QString &data)
{
	QWriteLocker w(&mCurrentLock);
	mCurrent = data;
	w.unlock();
	emit newData(mCurrent);
}

void Fifo::onReadError()
{
	mState.fail();
}
