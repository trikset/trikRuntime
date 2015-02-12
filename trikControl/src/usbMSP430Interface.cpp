/*
 * usbMSP430Interface.cpp
 *
 *  Created on: Fabruary 12, 2015
 *      Author: Rostislav Varzar
 */

#include <stdint.h>
#include <QByteArray>
#include "usbMSP430Interface.h"

void usb_msp_write(QByteArray const &i2c_data)
{
    if (i2c_data.size() == 2)
    {
        switch (i2c_data[0])
        {
            case MOT1:

                break;
            default:
                break;
        }
    }
    else
    {

    }
}

int32_t usb_msp_read(QByteArray const &i2c_data)
{
    if (i2c_data.size() == 2)
    {
        switch (i2c_data[0])
        {
            case ENC1:

                break;
            default:
                break;
        }
    }
    else
    {

    }
    return 0x00;
}
