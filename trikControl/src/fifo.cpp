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

	connect(mFifo.data(), &trikHal::FifoInterface::newData, this, &Fifo::onNewData); //Andrei
	connect(mFifo.data(), &trikHal::FifoInterface::newLine, this, &Fifo::onNewLine);
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
	if (mCurrentLine.isEmpty()) {
		r.unlock();
		QEventLoop l;
		connect(this, &Fifo::newLine, &l, [&l](const QString &newLine) { if (!newLine.isEmpty()) l.quit(); } );
		l.exec();
	}
	r.unlock();
	QString result;
	QWriteLocker w(&mCurrentLock);
	result.swap(mCurrentLine);
	return result;
}

QString Fifo::readRaw() //Andrei
{
	QReadLocker r(&mCurrentLock);
	if (mCurrentData.isEmpty()) {
		r.unlock();
		QEventLoop l;
		connect(this, &Fifo::newData, &l, [&l](const QVector<uint8_t> &newData) { if (!newData.isEmpty()) l.quit(); } );
		l.exec();
	}
	r.unlock();
	QString result;
	QWriteLocker w(&mCurrentLock);
	//result.swap(mCurrentData);
	return result;
}

bool Fifo::hasData() const
{
	QReadLocker r(&mCurrentLock);
	return !mCurrentLine.isEmpty();
}

void Fifo::onNewLine(const QString &line)
{
	QWriteLocker w(&mCurrentLock);
	mCurrentLine = line;
	w.unlock();
	emit newLine(mCurrentLine);
}

//void Fifo::onNewData(const QByteArray &data) //Andrei
void Fifo::onNewData(const QVector<uint8_t> &data) //Andrei
{
	QWriteLocker w(&mCurrentLock);
	mCurrentData = data;
	w.unlock();
	emit newData(mCurrentData);
}


void Fifo::onReadError()
{
	mState.fail();
}
