/* Copyright 2025 CyberTech Labs Ltd.
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

#include "commonI2c.h"
#include <fcntl.h>
#include "sys/ioctl.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <QsLog.h>
#include <QVector>

using namespace trikHal::trik;

void CommonI2c::disconnect()
{
	close(mDeviceFileDescriptor);
}

CommonI2c::~CommonI2c()
{
	disconnect();
}


bool CommonI2c::connect(const QString &devicePath, int deviceId)
{
	mDeviceFileDescriptor = open(devicePath.toStdString().c_str(), O_RDWR);

	if (mDeviceFileDescriptor < 0) {
		QLOG_ERROR() << "Failed to open I2C device file " << devicePath;
		return false;
	}

	if (ioctl(mDeviceFileDescriptor, I2C_SLAVE, deviceId)) {
		QLOG_ERROR() << "ioctl(" << mDeviceFileDescriptor << ", I2C_SLAVE, " << deviceId << ") failed ";
		return false;
	}

	mDeviceAddress = deviceId;
	return true;
}


QVector<uint8_t> CommonI2c::readX(const QByteArray &data)
{
	if (data.size() < 4) {
		return {};
	}

	struct i2c_msg i2c_messages[2];
	struct i2c_rdwr_ioctl_data i2c_messageset[1];

	char cmd[2] = {0, 0};

	if  (mRegSize == 2) {
		cmd[0] = data[1];
		cmd[1] = data[0];
	} else {
		cmd[0] = data[0];
	}

	i2c_messages[0] = {mDeviceAddress, 0, mRegSize, (__u8*)cmd};

	const auto  sizeForRead = static_cast<ushort>((data[3] << 8) | data[2]);
	QVector<uint8_t> vector(sizeForRead, 0);

	i2c_messages[1] = {mDeviceAddress, I2C_M_RD, sizeForRead, (__u8*)vector.data()};

	i2c_messageset[0].msgs = i2c_messages;
	i2c_messageset[0].nmsgs = 2;

	if (ioctl(mDeviceFileDescriptor, I2C_RDWR, &i2c_messageset) < 0) {
	      QLOG_INFO() << "Failed read dataSize" << sizeForRead;
	      return {};
	}

	return vector;
}

int CommonI2c::read(const QByteArray &data)
{
	if (data.size() < 4) {
		return -1;
	}

	const auto sizeForRead = (ushort)((data[3] << 8) | data[2]);
	auto vector = readX(data);

	if (vector.length() < sizeForRead) {
		return -1;
	}

	if (sizeForRead == 1) {
		return vector[0];
	}

	return (vector[1] << 8) | vector[0];
}

int CommonI2c::send(const QByteArray &data) {
	auto dataSize = data.size();

	if (dataSize < 3) {
		return -1;
	}

	if (mRegSize == 1) {
		if (dataSize == 4) {
			char cmd[3] = {data[0], data[3], data[2]};
			return write((__u8 *)cmd, sizeof(cmd));
		}
		char cmd[2] = {data[0], data[2]};
		return write((__u8 *)cmd, sizeof(cmd));
	}

	if (dataSize == 4) {
		char cmd[4] = {data[1], data[0], data[3], data[2]};
		return write((__u8 *)cmd, sizeof(cmd));
	}

	char cmd[3] = {data[1], data[0], data[2]};
	return write((__u8 *)cmd, sizeof(cmd));
}


int CommonI2c::write(__u8* writeData, __u16 length)
{
	struct i2c_msg i2c_messages[1];
	struct i2c_rdwr_ioctl_data i2c_messageset[1];

	i2c_messages[0] = {mDeviceAddress, 0, length, writeData};
	i2c_messageset[0].msgs = i2c_messages;
	i2c_messageset[0].nmsgs = 1;
	return ioctl(mDeviceFileDescriptor, I2C_RDWR, &i2c_messageset);
}

int CommonI2c::transfer(const QVector<MspI2cInterface::Message> &vector) {
	if (vector.size() > I2C_RDRW_IOCTL_MAX_MSGS) {
		return -1;
	}

	struct i2c_msg msgs[I2C_RDRW_IOCTL_MAX_MSGS];
	struct i2c_rdwr_ioctl_data rdwr {};

	int counter = 0;
	for (auto &&message: vector) {
		__u16 flags = 0;
		if (message.type == "read") {
			flags |= I2C_M_RD;
		}
		auto data = message.data;
		msgs[counter].addr = mDeviceAddress;
		msgs[counter].flags = flags;
		auto lenght = data.size();
		msgs[counter].len = lenght;
		msgs[counter].buf = (__u8*)data.data();
	}

	rdwr.msgs = msgs;
	rdwr.nmsgs = counter;
	return ioctl(mDeviceFileDescriptor, I2C_RDWR, &rdwr);
}
