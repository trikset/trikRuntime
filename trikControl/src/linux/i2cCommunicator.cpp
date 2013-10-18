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

#include <QtCore/QDebug>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>

 using namespace trikControl;

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
 	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
 		I2C_SMBUS_WORD_DATA,&data))
 		return -1;
 	else
 		return 0x0FFFF & data.word;
 }

 static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
 {
 	union i2c_smbus_data data;
 	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
 		I2C_SMBUS_BYTE_DATA,&data))
 		return -1;
 	else
 		return 0x0FFFF & data.byte;
 }

 static inline __s32 i2c_smbus_read_i2c_block_data(int file, __u8 command,
 	__u8 length, __u8 *values)
 {
 	union i2c_smbus_data data;
 	int i;

 	if (length > 32)
 		length = 32;
 	data.block[0] = length;
 	if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
 		length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN :
 		I2C_SMBUS_I2C_BLOCK_DATA,&data))
 		return -1;
 	else {
 		for (i = 1; i <= data.block[0]; i++)
 			values[i-1] = data.block[i];
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
 	return i2c_smbus_access(file,I2C_SMBUS_WRITE,command, I2C_SMBUS_BYTE_DATA, &data);
 }

static inline __s32 i2c_smbus_write_i2c_block_data(int file, __u8 command,
        __u8 length, __u8 *values)
{
        union i2c_smbus_data data;
        int i;
        if (length > 32)
                length = 32;        
        for (i = 1; i <= length; i++)
                data.block[i] = values[i-1];
        data.block[0] = length;
        return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
                I2C_SMBUS_I2C_BLOCK_DATA, &data);
}

 I2cCommunicator::I2cCommunicator(QString const &devicePath, int deviceId)
 : mDevicePath(devicePath)
 , mDeviceId(deviceId)
 {
 	connect();
 }

 I2cCommunicator::~I2cCommunicator()
 {
 	disconnect();
 }

 void I2cCommunicator::connect()
 {
 	mDeviceFileDescriptor = open(mDevicePath.toStdString().c_str(), O_RDWR);
 	if (mDeviceFileDescriptor < 0) {
 		qDebug() << "Failed to open I2C device file " << mDevicePath;
 		return;
 	}

 	if (ioctl(mDeviceFileDescriptor, I2C_SLAVE, mDeviceId)) {
 		qDebug() << "ioctl(" << mDeviceFileDescriptor << ", I2C_SLAVE, " << mDeviceId << ") failed ";
 		return;
 	}
 }

 void I2cCommunicator::send(QByteArray const &data,char reg)
 {
 	char count = data.size();
 	switch (count)
 	{
 		case 1:
 		{
 			i2c_smbus_write_byte_data(mDeviceFileDescriptor, reg, data[0]);
 			break;
 		}
 		case 2:
 		{
 			i2c_smbus_write_word_data(mDeviceFileDescriptor,reg, data[0] | (data[1] << 8));
 			break;
 		}
 		default:
 		{
 			__u8* cdata = QString(data).toAscii().data();
 			i2c_smbus_write_i2c_block_data(mDeviceFileDescriptor,reg,count,cdata);
 			break;
 		}
 	}
 }

 int I2cCommunicator::readToInt(char reg, int count)
 {
 	switch (count)
 	{
 		case 1:
 		{
 			return i2c_smbus_read_byte_data(mDeviceFileDescriptor, reg);
 			break;
 		} 
 		case 2:
 		{
 			return i2c_smbus_read_word_data(mDeviceFileDescriptor, reg);
 			break;
 		}
 		case 4:
 		{
 			__u8 buffer[count*sizeof(__u8)];
 			i2c_smbus_read_i2c_block_data(mDeviceFileDescriptor, reg, count, buffer);
 			return (buffer[3] << 24 | buffer[2] <<  16 | buffer[1] << 8 | buffer[0]);
 		}
 		default:
 		{
 			return 0xFFFFFFFF;
 			break;
 		}
 	} 

 }

QByteArray I2cCommunicator::readToByteArray(char reg, int count)
{
	__u8 buffer[count*sizeof(__u8)];
	i2c_smbus_read_i2c_block_data(mDeviceFileDescriptor, reg, count, buffer);
	return buffer; // convert it?
}

 void I2cCommunicator::disconnect()
 {
 	close(mDeviceFileDescriptor);
 }
