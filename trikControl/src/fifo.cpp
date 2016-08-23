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

	connect(mFifo.data(), SIGNAL(newData(QString)), this, SLOT(onNewData(QString)));
	connect(mFifo.data(), SIGNAL(readError()), this, SLOT(onReadError()));

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
	while (mCurrent.isEmpty()) {
		QEventLoop eventLoop;
		connect(this, SIGNAL(newData(QString)), &eventLoop, SLOT(quit()));
		eventLoop.exec();
	}

	const QString result = mCurrent;
	mCurrent = "";
	return result;
}

bool Fifo::hasData() const
{
	return mCurrent != "";
}

void Fifo::onNewData(const QString &data)
{
	QString buffer = data;
	mCurrent.swap(buffer);
	emit newData(data);
}

void Fifo::onReadError()
{
	mState.fail();
}
