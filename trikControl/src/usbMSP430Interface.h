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
uint32_t sendUSBPacket(char *usb_name, char *in_msp_packet, char *out_msp_packet);

/// Function for decoding received packet
uint32_t decodeReceivedPacket(char *msp_packet, uint8_t &dev_addr, uint8_t &func_code, uint8_t &reg_addr, uint32_t &reg_val);

/// Set power 12 volts on
void setPower12V(uint8_t pwr);

/// Configure USB stty device
void sttyUSBConfig(char *usb_name);

/// Connect to USB MSP430 device
void connect_USBMSP(FILE *&usb_out_descr, char *usb_name);

/// Disconnect from USB MSP430 device
void disconnect_USBMSP(FILE *&usb_out_descr, char *usb_name);

/// Send data to MSP430 via USB
void send_USBMSP(QByteArray const &i2c_data);

/// Read data from MSP430 via USB
uint32_t read_USBMSP(QByteArray const &i2c_data);


#endif // USBMSP430INTERFACE_H

