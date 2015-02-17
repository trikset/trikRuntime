/*
 * usbMSP430Interface.h
 *
 *  Created on: Fabruary 12, 2015
 *      Author: Rostislav Varzar
 */

#ifndef USBMSP430INTERFACE_H
#define USBMSP430INTERFACE_H

/// Extract number from packet
uint32_t hex2num(char *string, uint16_t pos, uint16_t numsize);

/// Make write register packet
void makeWriteRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr, uint32_t reg_val);

/// Make read register packet
void makeReadRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr);

/// Send USB packet
uint32_t sendUSBPacket(char *in_msp_packet, char *out_msp_packet);

/// Function for decoding received packet
uint32_t decodeReceivedPacket(char *msp_packet, uint8_t &dev_addr, uint8_t &func_code, uint8_t &reg_addr, uint32_t &reg_val);

/// Connect to USB MSP430 device
uint32_t connect_USBMSP();

/// Disconnect from USB MSP430 device
uint32_t disconnect_USBMSP();

/// Send data to MSP430 via USB
uint32_t send_USBMSP(QByteArray const &i2c_data);

/// Read data from MSP430 via USB
uint32_t read_USBMSP(QByteArray const &i2c_data);

#endif // USBMSP430INTERFACE_H

