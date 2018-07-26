/* Copyright 2018 Aleksey Fefelov and CyberTech Labs Ltd.
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

#include "i2cBusCommunicator.h"

using namespace trikControl;
using namespace trikHal;

#include <trikKernel/configurer.h>
#include <trikHal/mspI2cInterface.h>
#include <QsLog.h>


I2cBusCommunicator::I2cBusCommunicator(trikHal::MspI2cInterface &i2c, uint8_t bus, uint8_t deviceId)
	: mI2c(i2c)
	, mState("I2C Bus Communicator")
{
	QString devicePath; // = configurer.attributeByDevice("i2c", "path");
	if (bus == 1){
		devicePath = "/dev/i2c-1";
	} else if (bus == 2) {
		devicePath = "/dev/i2c-2";
	} else {
		QLOG_ERROR() << "Incorrect I2C bus " << bus; // << configurer.attributeByDevice("i2c", "deviceId");
		mState.fail();
		return;
	}

	if (mI2c.connect(devicePath, deviceId)) {
		mState.ready();
	} else {
		mState.fail();
	}
}

I2cBusCommunicator::~I2cBusCommunicator()
{
	if (mState.isReady()) {
		disconnect();
	}
}

void I2cBusCommunicator::send(const QByteArray &data)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to send data through I2C communicator which is not ready, ignoring";
		return;
	}

	QMutexLocker lock(&mLock);
	mI2c.send(data);
}

int I2cBusCommunicator::read(const QByteArray &data)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read data from I2C communicator which is not ready, ignoring";
		return 0;
	}

	QMutexLocker lock(&mLock);
	return mI2c.read(data);
}

void I2cBusCommunicator::disconnect()
{
	QMutexLocker lock(&mLock);
	mI2c.disconnect();
	mState.off();
}

DeviceInterface::Status I2cBusCommunicator::status() const
{
	return mState.status();
}
