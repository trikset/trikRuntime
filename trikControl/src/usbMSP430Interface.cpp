/*
 * usbMSP430Interface.cpp
 *
 *  Created on: Fabruary 12, 2015
 *      Author: Rostislav Varzar
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <QByteArray>
#include <QDebug>
#include "usbMSP430Interface.h"

/// Make write register packet
void makeWriteRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr, uint32_t reg_val)
{
    uint8_t crc = (0xFF - (dev_addr + WRITE_FUNC + reg_addr + uint8_t(reg_val & 0xFF) + uint8_t((reg_val >> 8) & 0xFF) +
                   uint8_t((reg_val >> 16) & 0xFF) + uint8_t((reg_val >> 24) & 0xFF)) + 1) & 0xFF;  //Checksum
    sprintf(msp_packet, ":%02X%02X%02X%08X%02X\n", dev_addr, WRITE_FUNC, reg_addr, reg_val, crc);
}

/// Make read register packet
void makeReadRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr)
{
    uint8_t crc = (0xFF - (dev_addr + READ_FUNC + reg_addr) + 1) & 0xFF;  //Checksum
    sprintf(msp_packet, ":%02X%02X%02X%02X\n", dev_addr, READ_FUNC, reg_addr, crc);
}

/// Function for decoding received packet
uint32_t decodeReceivedPacket(char *msp_packet, uint8_t &dev_addr, uint8_t &func_code, uint8_t &reg_addr, uint32_t &reg_val)
{
    if (msp_packet[0]!=':')   // Start condition error
        return START_ERROR;
    if ((strlen(msp_packet)!=9) && (strlen(msp_packet)!=17))   // Incorrect packet length
        return LENGTH_ERROR;
}

/// Write data to MSP430 via USB
void usb_msp_write(QByteArray const &i2c_data)
{
    if (i2c_data.size() == 2)
    {
        switch (i2c_data[0])
        {
            case i2cMOT1:

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
uint32_t usb_msp_read(QByteArray const &i2c_data)
{
    if (i2c_data.size() == 2)
    {
        switch (i2c_data[0])
        {
            case i2cENC1:

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
