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

#pragma once

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QMutex>

namespace trikControl {

/// Provides direct interaction with I2C device.
class I2cCommunicator
{
public:
	/// Constructor.
	/// @param devicePath - path to Linux I2C device file.
	/// @param deviceId - id of I2C device.
	I2cCommunicator(QString const &devicePath, int deviceId);

	~I2cCommunicator();

	/// Send data to current device, if it is connected.
	void send(QByteArray const &data);

	int read(QByteArray const &data);

private:
	/// Establish connection with current device.
	void connect();

	/// Disconnect from a device.
	void disconnect();

	QString const mDevicePath;
	int const mDeviceId;
	int mDeviceFileDescriptor;
	QMutex mLock;
};

}
