/*
 * usbMSP430Interface.h
 *
 *  Created on: Fabruary 12, 2015
 *      Author: Rostislav Varzar
 */

#ifndef USBMSP430INTERFACE_H
#define USBMSP430INTERFACE_H

/// I2C devices addresses
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

/// USB devices addresses
#define MOTOR1				0x00
#define MOTOR2				0x01
#define MOTOR3				0x02
#define MOTOR4				0x03
#define SENSOR1				0x04
#define SENSOR2				0x05
#define SENSOR3				0x06
#define SENSOR4				0x07
#define SENSOR5				0x08
#define SENSOR6				0x09
#define SENSOR7				0x0A
#define SENSOR8				0x0B
#define SENSOR9				0x0C
#define SENSOR10			0x0D
#define SENSOR11			0x0E
#define SENSOR12			0x0F
#define SENSOR13			0x10
#define SENSOR14			0x11
#define SENSOR15            0x12
#define SENSOR16            0x13
#define SENSOR17            0x14
#define SENSOR18            0x15
#define ENCODER1			0x16
#define ENCODER2			0x17
#define ENCODER3			0x18
#define ENCODER4			0x19
#define PORT1   			0x1A
#define PORT2	    		0x1B
#define PORT3		    	0x1C
#define PORT4			    0x1D
#define PORT5			    0x1E
#define PORT6			    0x1F
#define PORTJ			    0x20
#define PWM1                0x21
#define PWM2                0x22
#define PWM3                0x23
#define PWM4                0x24
#define SENSCTRL            0x25
#define ASYNCTIMER          0x26
#define TOUCHDEVICE         0x27
#define I2C1                0x28
#define I2C2                0x29
#define I2C3                0x2A
#define I2C4                0x2B
#define I2C5                0x2C
#define I2C6                0x2D
#define I2C7                0x2E
#define BSL                 0xEE
#define NNONE				0xFF

#define MAX_STRING_LENGTH   0x20

#define WRITE_FUNC          0x03
#define READ_FUNC           0x05

/// Error defines
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

/// HEX number sizes
#define NUM_BYTE            0x02
#define NUM_WORD            0x04
#define NUM_DWORD           0x08

#define RECV_PACK_LEN       0x12

/// Extract number from packet
uint32_t hex2num(char *string, uint16_t pos, uint16_t numsize);

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

