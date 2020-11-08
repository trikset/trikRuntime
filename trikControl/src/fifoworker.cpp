/* Copyright 2020 CyberTech Labs Ltd.
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

#include <QsLog.h>
#include <QtCore/QEventLoop>

#include "fifoworker.h"

trikControl::FifoWorker::FifoWorker(const QString &fileName
	, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mFifoFileName(fileName)
	, mHardwareAbstraction(hardwareAbstraction)
	, mState("Fifo on '" + fileName + "'")
{
	mWaitForInit.acquire(1);
}

trikControl::FifoWorker::~FifoWorker()
{
	mFifo.reset();
}

trikControl::DeviceInterface::Status trikControl::FifoWorker::status() const
{
	return mState.status();
}

void trikControl::FifoWorker::init()
{
	mFifo.reset(mHardwareAbstraction.createFifo(mFifoFileName));
	mState.start();

	connect(mFifo.data(), &trikHal::FifoInterface::newData, this, &FifoWorker::onNewData);
	connect(mFifo.data(), &trikHal::FifoInterface::newLine, this, &FifoWorker::onNewLine);
	connect(mFifo.data(), &trikHal::FifoInterface::readError, this, &FifoWorker::onReadError);

	if (mFifo->open()) {
		mState.ready();
	} else {
		mState.fail();
	}

	mWaitForInit.release(1);
}

QString trikControl::FifoWorker::read()
{
	QReadLocker r(&mCurrentLock);
	if (mCurrentLine.isEmpty()) {
		r.unlock();
		QEventLoop l;
		connect(this, &FifoWorker::newLine, &l, [&l](const QString &newLine) {
			if (!newLine.isEmpty()) {
				l.quit();
			}
		});
		l.exec();
	}
	r.unlock();
	QString result;
	QWriteLocker w(&mCurrentLock);
	result.swap(mCurrentLine);
	return result;
}

QVector<uint8_t> trikControl::FifoWorker::readRaw()
{
	QReadLocker r(&mCurrentLock);
	if (mCurrentData.isEmpty()) {
		r.unlock();
		QEventLoop l;
		connect(this, &FifoWorker::newData, &l, [&l](const QVector<uint8_t> &newData) {
			if (!newData.isEmpty()) {
				l.quit();
			}
		});
		l.exec();
	}
	r.unlock();
	QVector<uint8_t> result;
	QWriteLocker w(&mCurrentLock);
	result.swap(mCurrentData);
	return result;
}

bool trikControl::FifoWorker::hasLine() const
{
	QReadLocker r(&mCurrentLock);
	return !mCurrentLine.isEmpty();
}

bool trikControl::FifoWorker::hasData() const
{
	QReadLocker r(&mCurrentLock);
	return !mCurrentData.isEmpty();
}

void trikControl::FifoWorker::waitUntilInited()
{
	mWaitForInit.acquire(1);
	mWaitForInit.release(1);
}

void trikControl::FifoWorker::onNewLine(const QString &line)
{
	QWriteLocker w(&mCurrentLock);
	mCurrentLine = line;
	w.unlock();
	emit newLine(mCurrentLine);
}

void trikControl::FifoWorker::onNewData(const QVector<uint8_t> &data)
{
	QWriteLocker w(&mCurrentLock);
	mCurrentData.append(data);
	if (mCurrentData.size() > 1024 * 1024) {
		QLOG_ERROR() << "FIFO buffer limit exceeded, buffer droped. Use readRaw more often";
		mCurrentData.clear();
	}
	w.unlock();
	emit newData(mCurrentData);
}

void trikControl::FifoWorker::onReadError()
{
	mState.fail();
}
