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

#include "src/mspUsbCommunicator.h"

#include <trikKernel/configurer.h>
#include <trikHal/mspUsbInterface.h>

#include <QsLog.h>

using namespace trikControl;

MspUsbCommunicator::MspUsbCommunicator(trikHal::MspUsbInterface &usb)
	: mUsb(usb)
	, mState("MSP USB Communicator")
{
	if (mUsb.connect()) {
		mState.ready();
	} else {
		mState.fail();
	}
}

MspUsbCommunicator::~MspUsbCommunicator()
{
	if (mState.isReady()) {
		disconnect();
	}
}

int MspUsbCommunicator::send(const QByteArray &data)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to send data through USB I2C communicator which is not ready, ignoring";
		return -1;
	}

	QMutexLocker lock(&mLock);
	return mUsb.send(data);
}

int MspUsbCommunicator::read(const QByteArray &data)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read data from USB I2C communicator which is not ready, ignoring";
		return 0;
	}

	QMutexLocker lock(&mLock);
	return mUsb.read(data);
}

QVector<uint8_t> MspUsbCommunicator::readX(const QByteArray &data)
{
	Q_UNUSED(data);
	QLOG_INFO() << "readX not supported for MspUsbCommunicator";
	return {};
}

DeviceInterface::Status MspUsbCommunicator::status() const
{
	return mState.status();
}

void MspUsbCommunicator::disconnect()
{
	mState.stop();
	QMutexLocker lock(&mLock);
	mUsb.disconnect();
	mState.off();
}
