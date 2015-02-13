/*
 * usbMSP430Interface.h
 *
 *  Created on: Fabruary 12, 2015
 *      Author: Rostislav Varzar
 */

#ifndef USBMSP430INTERFACE_H
#define USBMSP430INTERFACE_H

/// Devices addresses
#define i2cMOT1             0x14
#define i2cMOT2             0x15
#define i2cMOT3             0x17
#define i2cMOT4             0x16
#define i2cBATT             0x26
#define i2cSENS1            0x25
#define i2cSENS2            0x24
#define i2cSENS3            0x23
#define i2cSENS4            0x22
#define i2cSENS5            0x21
#define i2cSENS6            0x20
#define i2cENC1             0x30
#define i2cENC2             0x31
#define i2cENC3             0x32
#define i2cENC4             0x33

#define MAX_STRING_LENGTH   0x20

#define WRITE_FUNC          0x03
#define READ_FUNC           0x05

//Error defines
#define NO_ERROR            0x00
#define FUNC_CODE_ERROR     0x01
#define REG_ADDR_ERROR      0x02
#define REG_VAL_ERROR       0x03
#define SLAVE_ERROR         0x04
#define SLAVE_BUSY          0x06
#define DEV_ADDR_ERROR      0x11
#define CRC_ERROR           0x12
#define START_ERROR         0x13
#define LENGTH_ERROR        0x14
#define REG_INC_ERROR       0x15
#define UNDEF_ERROR         0xFF

/// Make write register packet
void makeWriteRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr, uint32_t reg_val);

/// Make read register packet
void makeReadRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr);

/// Function for decoding received packet
uint32_t decodeReceivedPacket(char *msp_packet, uint8_t &dev_addr, uint8_t &func_code, uint8_t &reg_addr, uint32_t &reg_val);

/// Write data to MSP430 via USB
void usb_msp_write(QByteArray const &i2c_data);

/// Read data from MSP430 via USB
uint32_t usb_msp_read(QByteArray const &i2c_data);

#endif // USBMSP430INTERFACE_H

