/* Copyright 2013 Yurii Litvinov
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

#include "src/i2cCommunicator.h"

#include <trikKernel/configurer.h>

#include <trikHal/i2cInterface.h>
#include <QsLog.h>

using namespace trikControl;

I2cCommunicator::I2cCommunicator(const trikKernel::Configurer &configurer, trikHal::I2CInterface &i2c)
	: mI2c(i2c)
{
	const QString devicePath = configurer.attributeByDevice("i2c", "path");

	bool ok = false;
	const int deviceId = configurer.attributeByDevice("i2c", "deviceId").toInt(&ok, 0);
	if (!ok) {
		QLOG_ERROR() << "Incorrect I2C device id" << configurer.attributeByDevice("i2c", "deviceId");
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

DeviceInterface::Status I2cCommunicator::status() const
{
	return mState.status();
}

void I2cCommunicator::disconnect()
{
	QMutexLocker lock(&mLock);
	mI2c.disconnect();
	mState.off();
}
