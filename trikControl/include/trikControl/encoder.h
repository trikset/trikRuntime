#pragma once

#define I2C_SLAVE	0x0703	/* Use this slave address */
#define I2C_SLAVE_FORCE	0x0706	/* Use this slave address, even if it */
#define I2C_SMBUS	0x0720	/* SMBus transfer */
#define ADDRESS 0x48

#include <QtCore/QObject>

#include <sys/ioctl.h>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <linux/input.h>
#include <stdio.h>
#include <linux/types.h>
#include <linux/i2c.h>

#include "declSpec.h"

namespace trikControl {

class I2cCommunicator;

class TRIKCONTROL_EXPORT Encoder : public QObject
{
    Q_OBJECT

public:
    Encoder(I2cCommunicator &communicator, int jbx);

public slots:
    float get();
    void reset();

private slots:

private:
    I2cCommunicator &mCommunicator;
    float mData;
    int mJbx;

private:
    struct i2c_smbus_ioctl_data {
        char read_write;
        __u8 command;
        int size;
        union i2c_smbus_data *data;
    };

    /* This is the structure as used in the I2C_RDWR ioctl call */
    struct i2c_rdwr_ioctl_data {
        struct i2c_msg *msgs;	/* pointers to i2c_msgs */
        int nmsgs;		/* number of i2c_msgs */
    };

    static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,
                                         int size, union i2c_smbus_data *data)
    {
        struct i2c_smbus_ioctl_data args;

        args.read_write = read_write;
        args.command = command;
        args.size = size;
        args.data = data;
        return ioctl(file,I2C_SMBUS,&args);
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

    static inline __s32 i2c_smbus_read_block_data(int file, __u8 command,
                                                  __u8 *values)
    {
        union i2c_smbus_data data;
        int i;
        if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
                             I2C_SMBUS_BLOCK_DATA,&data))
            return -1;
        else {
            for (i = 1; i <= data.block[0]; i++)
                values[i-1] = data.block[i];
                return data.block[0];
        }
    }
};

}
