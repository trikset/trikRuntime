/*
 * usbMSP430Interface.cpp
 *
 *  Created on: Fabruary 12, 2015
 *      Author: Rostislav Varzar
 */

#include <stdint.h>
#include <QByteArray>
#include "usbMSP430Interface.h"

/// Converts number to ASCII code according to HEX format
uint8_t to_hex(uint8_t i)
{
    return (i <= 9 ? '0' + i : 'A' - 10 + i);
}

/// Converts number to HEX string
void num2hex(char *string, uint8_t number)
{
    string[0] = to_hex((number & 0x00F0) >> 4);
    string[1] = to_hex(number & 0x000F);
    string[2] = '\0';
}



/// Write data to MSP430 via USB
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

/// Read data from MSP430 via USB
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
