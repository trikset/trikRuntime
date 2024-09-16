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

#include "i2cCommunicator.h"

using namespace trikControl;
using namespace trikHal;

#include <QsLog.h>

#include <trikKernel/configurer.h>
#include <trikHal/mspI2cInterface.h>

I2cCommunicator::I2cCommunicator(const trikKernel::Configurer &configurer, trikHal::MspI2cInterface &i2c
	, uint8_t bus, uint8_t deviceId)
	: mI2c(i2c)
	, mState("I2C Communicator")
{
	QString devicePath;
	if (bus == 1) {
		devicePath = configurer.attributeByDevice("i2cBus1", "path");
	} else if (bus == 2) {
		devicePath = configurer.attributeByDevice("i2cBus2", "path");
	} else {
		QLOG_ERROR() << "Incorrect I2C bus " << bus;
		mState.fail();
		return;
	}

	if (mI2c.connect(devicePath, deviceId)) {
		mState.ready();
	} else {
		mState.fail();
	}
}

I2cCommunicator::~I2cCommunicator()
{
	if (mState.isReady()) {
		disconnect();
	}
}

void I2cCommunicator::send(const QByteArray &data)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to send data through I2C communicator which is not ready, ignoring";
		return;
	}

	QMutexLocker lock(&mLock);
	mI2c.send(data);
}

int I2cCommunicator::read(const QByteArray &data)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read data from I2C communicator which is not ready, ignoring";
		return 0;
	}

	QMutexLocker lock(&mLock);
	return mI2c.read(data);
}

void I2cCommunicator::disconnect()
{
	QMutexLocker lock(&mLock);
	mI2c.disconnect();
	mState.off();
}

DeviceInterface::Status I2cCommunicator::status() const
{
	return mState.status();
}
