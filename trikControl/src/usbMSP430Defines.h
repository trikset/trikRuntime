/*
 * usbMSP430Interface.h
 *
 *  Created on: Fabruary 17, 2015
 *	Author: Rostislav Varzar
 */

#ifndef USBMSP430DEFINES_H
#define USBMSP430DEFINES_H

/// I2C devices addresses
#define i2cPWMMOT1		0x10	// 16 dec
#define i2cPWMMOT2		0x11	// 17 dec
#define i2cPWMMOT3		0x13	// 19 dec
#define i2cPWMMOT4		0x12	// 18 dec
#define i2cMOT1			0x14	// 20 dec
#define i2cMOT2			0x15	// 21 dec
#define i2cMOT3			0x17	// 23 dec
#define i2cMOT4			0x16	// 22 dec
#define i2cBATT			0x26	// 38 dec
#define i2cSENS1		0x25	// 37 dec
#define i2cSENS2		0x24	// 36 dec
#define i2cSENS3		0x23	// 35 dec
#define i2cSENS4		0x22	// 34 dec
#define i2cSENS5		0x21	// 33 dec
#define i2cSENS6		0x20	// 32 dec
#define i2cENC1			0x30	// 48 dec
#define i2cENC2			0x31	// 49 dec
#define i2cENC3			0x32	// 50 dec
#define i2cENC4			0x33	// 51 dec
#define i2cSERV1		0x3F	// 63 dec
#define i2cSERV2		0x40	// 64 dec
#define i2cSERV3		0x41	// 65 dec
#define i2cSERV4		0x42	// 66 dec
#define i2cSERV5		0x43	// 67 dec
#define i2cSERV6		0x44	// 68 dec
#define i2cSERV7		0x45	// 69 dec
#define i2cSERV8		0x46	// 70 dec
#define i2cSERV9		0x47	// 71 dec
#define i2cSERV10		0x48	// 72 dec
#define i2cSERV11		0x49	// 73 dec
#define i2cSERV12		0x4A	// 74 dec
#define i2cSERV13		0x4B	// 75 dec
#define i2cSERV14		0x4C	// 76 dec
#define i2cTEMP1		0x4D	// 77 dec
#define i2cTEMP2		0x4E	// 78 dec
#define i2cTEMP3		0x4F	// 79 dec
#define i2cW1			0x50	// 80 dec
#define i2cW2			0x51	// 81 dec
#define i2cW3			0x52	// 82 dec
#define i2cW4			0x53	// 83 dec
#define i2cU1_0x11		0x54	// 84 dec
#define i2cU1_0x12		0x55
#define i2cU1_0x13		0x56
#define i2cU1_0x14		0x57
#define i2cU1_0x15		0x58
#define i2cU1_0x16		0x59
#define i2cU1_0x17		0x5A
#define i2cU1_0x18		0x5B
#define i2cU1_0x19		0x5C
#define i2cU1_0x1A		0x5D
#define i2cU1_0x1B		0x5E
#define i2cU1_0x1C		0x5F
#define i2cU1_0x1D		0x60
#define i2cU1_0x1E		0x61
#define i2cU1_0x1F		0x62
#define i2cU1_0x20		0x63
#define i2cU2_0x11		0x64	// 100 dec
#define i2cU2_0x12		0x65
#define i2cU2_0x13		0x66
#define i2cU2_0x14		0x67
#define i2cU2_0x15		0x68
#define i2cU2_0x16		0x69
#define i2cU2_0x17		0x6A
#define i2cU2_0x18		0x6B
#define i2cU2_0x19		0x6C
#define i2cU2_0x1A		0x6D
#define i2cU2_0x1B		0x6E
#define i2cU2_0x1C		0x6F
#define i2cU2_0x1D		0x70
#define i2cU2_0x1E		0x71
#define i2cU2_0x1F		0x72
#define i2cU2_0x20		0x73
#define i2cU3_0x11		0x74	// 116 dec
#define i2cU3_0x12		0x75
#define i2cU3_0x13		0x76
#define i2cU3_0x14		0x77
#define i2cU3_0x15		0x78
#define i2cU3_0x16		0x79
#define i2cU3_0x17		0x7A
#define i2cU3_0x18		0x7B
#define i2cU3_0x19		0x7C
#define i2cU3_0x1A		0x7D
#define i2cU3_0x1B		0x7E
#define i2cU3_0x1C		0x7F
#define i2cU3_0x1D		0x80
#define i2cU3_0x1E		0x81
#define i2cU3_0x1F		0x82
#define i2cU3_0x20		0x83
#define i2cU4_0x11		0x84	// 132 dec
#define i2cU4_0x12		0x85
#define i2cU4_0x13		0x86
#define i2cU4_0x14		0x87
#define i2cU4_0x15		0x88
#define i2cU4_0x16		0x89
#define i2cU4_0x17		0x8A
#define i2cU4_0x18		0x8B
#define i2cU4_0x19		0x8C
#define i2cU4_0x1A		0x8D
#define i2cU4_0x1B		0x8E
#define i2cU4_0x1C		0x8F
#define i2cU4_0x1D		0x90
#define i2cU4_0x1E		0x91
#define i2cU4_0x1F		0x92
#define i2cU4_0x20		0x93
#define i2cU5_0x11		0x94	// 148 dec
#define i2cU5_0x12		0x95
#define i2cU5_0x13		0x96
#define i2cU5_0x14		0x97
#define i2cU5_0x15		0x98
#define i2cU5_0x16		0x99
#define i2cU5_0x17		0x9A
#define i2cU5_0x18		0x9B
#define i2cU5_0x19		0x9C
#define i2cU5_0x1A		0x9D
#define i2cU5_0x1B		0x9E
#define i2cU5_0x1C		0x9F
#define i2cU5_0x1D		0xA0
#define i2cU5_0x1E		0xA1
#define i2cU5_0x1F		0xA2
#define i2cU5_0x20		0xA3
#define i2cU6_0x11		0xA4	// 164 dec
#define i2cU6_0x12		0xA5
#define i2cU6_0x13		0xA6
#define i2cU6_0x14		0xA7
#define i2cU6_0x15		0xA8
#define i2cU6_0x16		0xA9
#define i2cU6_0x17		0xAA
#define i2cU6_0x18		0xAB
#define i2cU6_0x19		0xAC
#define i2cU6_0x1A		0xAD
#define i2cU6_0x1B		0xAE
#define i2cU6_0x1C		0xAF
#define i2cU6_0x1D		0xB0
#define i2cU6_0x1E		0xB1
#define i2cU6_0x1F		0xB2
#define i2cU6_0x20		0xB3
#define i2cU7_0x11		0xB4	// 180 dec
#define i2cU7_0x12		0xB5
#define i2cU7_0x13		0xB6
#define i2cU7_0x14		0xB7
#define i2cU7_0x15		0xB8
#define i2cU7_0x16		0xB9
#define i2cU7_0x17		0xBA
#define i2cU7_0x18		0xBB
#define i2cU7_0x19		0xBC
#define i2cU7_0x1A		0xBD
#define i2cU7_0x1B		0xBE
#define i2cU7_0x1C		0xBF
#define i2cU7_0x1D		0xC0
#define i2cU7_0x1E		0xC1
#define i2cU7_0x1F		0xC2
#define i2cU7_0x20		0xC3	// 195 dec

/// USB devices addresses
#define MOTOR1			0x00
#define MOTOR2			0x01
#define MOTOR3			0x03
#define MOTOR4			0x02
#define SENSOR1			0x04
#define SENSOR2			0x05
#define SENSOR3			0x06
#define SENSOR4			0x07
#define SENSOR5			0x08
#define SENSOR6			0x09
#define SENSOR7			0x0A
#define SENSOR8			0x0B
#define SENSOR9			0x0C
#define SENSOR10		0x0D
#define SENSOR11		0x0E
#define SENSOR12		0x0F
#define SENSOR13		0x10
#define SENSOR14		0x11
#define SENSOR15		0x12
#define SENSOR16		0x13
#define SENSOR17		0x14
#define SENSOR18		0x15
#define ENCODER1		0x16
#define ENCODER2		0x17
#define ENCODER3		0x19
#define ENCODER4		0x18
#define PORT1			0x1A
#define PORT2			0x1B
#define PORT3			0x1C
#define PORT4			0x1D
#define PORT5			0x1E
#define PORT6			0x1F
#define PORTJ			0x20
#define PWM1			0x21
#define PWM2			0x22
#define PWM3			0x23
#define PWM4			0x24
#define VERSIONCTRL		0x25
#define ASYNCTIMER		0x26
#define TOUCHDEVICE		0x27
#define I2C1			0x28
#define I2C2			0x29
#define I2C3			0x2A
#define I2C4			0x2B
#define I2C5			0x2C
#define I2C6			0x2D
#define I2C7			0x2E
#define SPWM1			0x2F
#define SPWM2			0x30
#define SPWM3			0x31
#define SPWM4			0x32
#define SPWM5			0x33
#define SPWM6			0x34
#define SPWM7			0x35
#define SPWM8			0x36
#define SPWM9			0x37
#define SPWM10			0x38
#define SPWM11			0x39
#define SPWM12			0x3A
#define SPWM13			0x3B
#define SPWM14			0x3C
#define USART1			0x3D
#define USART2			0x3E
#define USART3			0x3F
#define USART4			0x40
#define USART5			0x41
#define USART6			0x42
#define USART7			0x43
#define BSL			0xEE
#define NNONE			0xFF

/// Motor registers
#define MMCTL			0x00
#define MMDUT			0x01
#define MMPER			0x02
#define MMANG			0x03
#define MMTMR			0x04
#define MMVAL			0x05
#define MMERR			0x06

/// Masks for bits of control register
#define MOT_ENABLE		0x8000	// Motor enable
#define MOT_AUTO		0x4000	// Autostop mode
#define MOT_ANGLE		0x2000	// Angle autostop mode
#define MOT_BACK		0x0010	// Backward direction
#define MOT_BRAKE		0x0008	// Brake enable
#define MOT_POWER		0x0003	// Motor power

/// Default motor PWM period
#define DEF_MOT_PER		0x3000	// Default motor period in uS

/// Encoder registers
#define EECTL			0x00
#define EEVAL			0x01
#define EESTA			0x02

/// Masks for bits of control register
#define ENC_ENABLE		0x8000	// Enable bit
#define ENC_ASYNC		0x4000	// Async mode
#define ENC_2WIRES		0x2000	// 2-wires mode
#define ENC_PUPEN		0x1000	// Pull up resistors
#define ENC_FALL		0x0800	// Falling edge select

/// Sensor registers
#define SSCTL			0x00
#define SSIDX			0x01
#define SSVAL			0x02
#define SSSTA			0x03

/// Sensor types
#define DIGITAL_INP		0x0000
#define ANALOG_INP		0x0001

/// Masks for bits of control register
#define SENS_ENABLE		0x8000	// Enable bit
#define SENS_ASYNC		0x4000	// Async mode
#define SENS_PULL		0x2000	// Pull up resistors
#define SENS_READ		0x0001	// Sensor read

/// Software I2C registers
#define IICTL			0x00
#define IIDEV			0x01
#define IIREG			0x02
#define IIDAT			0x03
#define IIERR			0x04
#define IIIDX			0x05
#define IIVAL			0x06
#define IIDEL			0x07
#define IIVER			0x08
#define IIPAR			0x09

/// Masks for bits of control register
#define I2C_ENABLE		0x80	// I2C enable
#define I2C_READ		0x20	// I2C read
#define I2C_WRITE		0x40	// I2C write
#define I2C_SENS		0x10	// I2C read sensor
#define I2C_PULL		0x08	// I2C pullup resistors

/// USART registers
#define UUCTL			0x00
#define UUSPD			0x01
#define UUSTA			0x02
#define UUDAT			0x03

/// Masks for bits of control register
#define USART_EN		0x80000000 // USART enable
#define USART_EVEN		0x40000000 // USART even parity
#define USART_ODD		0x20000000 // USART odd parity
#define USART_2SB		0x10000000 // USART 1.5 or 2 stop bits
#define USART_6BITS		0x04000000 // USART 6 bits word length
#define USART_7BITS		0x08000000 // USART 7 bits word length
#define USART_8BITS		0x0C000000 // USART 8 bits word length
#define USART_IRDA1		0x00800000 // USART IrDA 115.2 kbaud mode
#define USART_IRDA2		0x01000000 // USART IrDA 1.152 Mbaud mode
#define USART_RS485		0x02000000 // USART RS-485 mode
#define USART_INVRTS		0x00400000 // USART RTS inverted mode
#define USART_RXEN		0x00200000 // USART receiver enable
#define USART_TXEN		0x00100000 // USART transmitter enable
#define USART_RTS		0x00080000 // USART RTS high level
#define USART_DTR		0x00040000 // USART DTR high level
#define USART_RST		0x00000003 // USART software reset

/// Software I2C sensors addresses
#define NXTTEMP			0x0000
#define HMC5883L_X		0x0001
#define HMC5883L_Y		0x0002
#define HMC5883L_Z		0x0003
#define MCP3424_CH1		0x0004
#define MCP3424_CH2		0x0005
#define MCP3424_CH3		0x0006
#define MCP3424_CH4		0x0007

/// MCP3424 PGA gain
#define MCP3424_GAIN1		0x0000
#define MCP3424_GAIN2		0x0001
#define MCP3424_GAIN4		0x0002
#define MCP3424_GAIN8		0x0003

/// Software PWM registers
#define SPPCTL			0x00
#define SPPDUT			0x01
#define SPPPER			0x02
#define SPPVER			0x03

/// Masks for bits of control register
#define SPWM_ENABLE		0x8000	// SPWM enable

/// Default software PWM period
#define DEF_SPWM_PER		0x00C8	// Default software PWM period (for servo motors), 50 Hz (20 ms)

/// Ranges of servo motors PWM duty
#define MIN_SERV_DUTY		0x0007
#define MAX_SERV_DUTY		0x0017

#define MAX_STRING_LENGTH	0x20

#define WRITE_FUNC		0x03
#define READ_FUNC		0x05

/// Error defines
#define NO_ERROR		0x00
#define FUNC_CODE_ERROR		0x01
#define REG_ADDR_ERROR		0x02
#define REG_VAL_ERROR		0x03
#define SLAVE_ERROR		0x04
#define SLAVE_BUSY		0x06
#define DEV_ADDR_ERROR		0x11
#define CRC_ERROR		0x12
#define START_ERROR		0x13
#define LENGTH_ERROR		0x14
#define REG_INC_ERROR		0x15
#define UNDEF_ERROR		0xFF

/// HEX number sizes
#define NUM_BYTE		0x02
#define NUM_WORD		0x04
#define NUM_DWORD		0x08

#define RECV_PACK_LEN		0x12

/// Errors for response
#define NO_ERROR		0x00
#define DEVICE_ERROR		0x01
#define PACKET_ERROR		0x02

#define TIME_OUT		0xFFFF

/// Alternative functions of devices
#define ALT_NOTHING		0x00
#define ALT_ANALOG		0x01
#define ALT_I2C			0x02
#define ALT_SERVO		0x03
#define ALT_ENC			0x04
#define ALT_USART		0x05

/// USB device file
#define USB_DEV_NAME		"/dev/ttyACM0"

#endif // USBMSP430DEFINES_H

