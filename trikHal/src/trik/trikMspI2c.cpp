/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "trikMspI2c.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>

#include <QsLog.h>

using namespace trikHal::trik;

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command
		, int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file, I2C_SMBUS, &args);
}

static inline __s32 i2c_smbus_read_word_data(int file, __u8 command)
{
	union i2c_smbus_data data;
	if (i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_WORD_DATA, &data)) {
		return -1;
	} else {
		return 0x0FFFF & data.word;
	}
}

static inline __s32 i2c_smbus_read_i2c_block_data(int file, __u8 command, __u8 length, __u8 *values)
{
	union i2c_smbus_data data;

	if (length > 32) {
		length = 32;
	}

	data.block[0] = length;
	if (i2c_smbus_access(file, I2C_SMBUS_READ, command
			, length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN : I2C_SMBUS_I2C_BLOCK_DATA
			, &data))
	{
			return -1;
	} else {
			for (int i = 1; i <= data.block[0]; i++) {
					values[i - 1] = data.block[i];
			}

			return data.block[0];
	}
}

static inline __s32 i2c_smbus_write_word_data(int file, __u8 command, __u16 value)
{
	union i2c_smbus_data data;
	data.word = value;
	return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_WORD_DATA, &data);
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &data);
}

TrikMspI2c::~TrikMspI2c()
{
	disconnect();
}

void TrikMspI2c::send(const QByteArray &data)
{
	qDebug() << "TrikMspI2c::send" << (int)data[0] << (int)data[1] << (int)data[2];
	if (data.size() == 3) {
		i2c_smbus_write_byte_data(mDeviceFileDescriptor, data[0], data[2]);
	} else {
		i2c_smbus_write_word_data(mDeviceFileDescriptor, data[0], data[2] | (data[3] << 8));
	}
}

int TrikMspI2c::read(const QByteArray &data)
{
	if (data.size() == 2) {
		return i2c_smbus_read_word_data(mDeviceFileDescriptor, data[0]);
	} else {
		__u8 buffer[4] = {0};
		i2c_smbus_read_i2c_block_data(mDeviceFileDescriptor, data[0], 4, buffer);
		return buffer[3] << 24 | buffer[2] <<  16 | buffer[1] << 8 | buffer[0];
	}
}

bool TrikMspI2c::connect(const QString &devicePath, int deviceId)
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

	return true;
}

void TrikMspI2c::disconnect()
{
	close(mDeviceFileDescriptor);
}
