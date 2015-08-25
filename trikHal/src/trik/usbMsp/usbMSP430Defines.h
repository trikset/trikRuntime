/* Copyright 2015 Rostislav Varzar
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
 * limitations under the License.
 *  Created on: Fabruary 17, 2015
 *	Author: Rostislav Varzar
*/

#pragma once

/// I2C devices addresses
#define i2cPWMMOT1		0x0010	// 16 dec
#define i2cPWMMOT2		0x0011	// 17 dec
#define i2cPWMMOT3		0x0013	// 19 dec
#define i2cPWMMOT4		0x0012	// 18 dec
#define i2cMOT1			0x0014	// 20 dec
#define i2cMOT2			0x0015	// 21 dec
#define i2cMOT3			0x0017	// 23 dec
#define i2cMOT4			0x0016	// 22 dec
#define i2cBATT			0x0026	// 38 dec
#define i2cSENS1		0x0025	// 37 dec
#define i2cSENS2		0x0024	// 36 dec
#define i2cSENS3		0x0023	// 35 dec
#define i2cSENS4		0x0022	// 34 dec
#define i2cSENS5		0x0021	// 33 dec
#define i2cSENS6		0x0020	// 32 dec
#define i2cENC1			0x0030	// 48 dec
#define i2cENC2			0x0031	// 49 dec
#define i2cENC3			0x0032	// 50 dec
#define i2cENC4			0x0033	// 51 dec
#define i2cSERV1		0x003F	// 63 dec
#define i2cSERV2		0x0040	// 64 dec
#define i2cSERV3		0x0041	// 65 dec
#define i2cSERV4		0x0042	// 66 dec
#define i2cSERV5		0x0043	// 67 dec
#define i2cSERV6		0x0044	// 68 dec
#define i2cSERV7		0x0045	// 69 dec
#define i2cSERV8		0x0046	// 70 dec
#define i2cSERV9		0x0047	// 71 dec
#define i2cSERV10		0x0048	// 72 dec
#define i2cSERV11		0x0049	// 73 dec
#define i2cSERV12		0x004A	// 74 dec
#define i2cSERV13		0x004B	// 75 dec
#define i2cSERV14		0x004C	// 76 dec
#define i2cTEMP1		0x004D	// 77 dec
#define i2cTEMP2		0x004E	// 78 dec
#define i2cTEMP3		0x004F	// 79 dec
#define i2cW1			0x0050	// 80 dec
#define i2cW2			0x0051	// 81 dec
#define i2cW3			0x0052	// 82 dec
#define i2cW4			0x0053	// 83 dec
#define i2cU1_0x11		0x0054	// 84 dec
#define i2cU1_0x12		0x0055
#define i2cU1_0x13		0x0056
#define i2cU1_0x14		0x0057
#define i2cU1_0x15		0x0058
#define i2cU1_0x16		0x0059
#define i2cU1_0x17		0x005A
#define i2cU1_0x18		0x005B
#define i2cU1_0x19		0x005C
#define i2cU1_0x1A		0x005D
#define i2cU1_0x1B		0x005E
#define i2cU1_0x1C		0x005F
#define i2cU1_0x1D		0x0060
#define i2cU1_0x1E		0x0061
#define i2cU1_0x1F		0x0062
#define i2cU1_0x20		0x0063
#define i2cU2_0x11		0x0064	// 100 dec
#define i2cU2_0x12		0x0065
#define i2cU2_0x13		0x0066
#define i2cU2_0x14		0x0067
#define i2cU2_0x15		0x0068
#define i2cU2_0x16		0x0069
#define i2cU2_0x17		0x006A
#define i2cU2_0x18		0x006B
#define i2cU2_0x19		0x006C
#define i2cU2_0x1A		0x006D
#define i2cU2_0x1B		0x006E
#define i2cU2_0x1C		0x006F
#define i2cU2_0x1D		0x0070
#define i2cU2_0x1E		0x0071
#define i2cU2_0x1F		0x0072
#define i2cU2_0x20		0x0073
#define i2cU3_0x11		0x0074	// 116 dec
#define i2cU3_0x12		0x0075
#define i2cU3_0x13		0x0076
#define i2cU3_0x14		0x0077
#define i2cU3_0x15		0x0078
#define i2cU3_0x16		0x0079
#define i2cU3_0x17		0x007A
#define i2cU3_0x18		0x007B
#define i2cU3_0x19		0x007C
#define i2cU3_0x1A		0x007D
#define i2cU3_0x1B		0x007E
#define i2cU3_0x1C		0x007F
#define i2cU3_0x1D		0x0080
#define i2cU3_0x1E		0x0081
#define i2cU3_0x1F		0x0082
#define i2cU3_0x20		0x0083
#define i2cU4_0x11		0x0084	// 132 dec
#define i2cU4_0x12		0x0085
#define i2cU4_0x13		0x0086
#define i2cU4_0x14		0x0087
#define i2cU4_0x15		0x0088
#define i2cU4_0x16		0x0089
#define i2cU4_0x17		0x008A
#define i2cU4_0x18		0x008B
#define i2cU4_0x19		0x008C
#define i2cU4_0x1A		0x008D
#define i2cU4_0x1B		0x008E
#define i2cU4_0x1C		0x008F
#define i2cU4_0x1D		0x0090
#define i2cU4_0x1E		0x0091
#define i2cU4_0x1F		0x0092
#define i2cU4_0x20		0x0093
#define i2cU5_0x11		0x0094	// 148 dec
#define i2cU5_0x12		0x0095
#define i2cU5_0x13		0x0096
#define i2cU5_0x14		0x0097
#define i2cU5_0x15		0x0098
#define i2cU5_0x16		0x0099
#define i2cU5_0x17		0x009A
#define i2cU5_0x18		0x009B
#define i2cU5_0x19		0x009C
#define i2cU5_0x1A		0x009D
#define i2cU5_0x1B		0x009E
#define i2cU5_0x1C		0x009F
#define i2cU5_0x1D		0x00A0
#define i2cU5_0x1E		0x00A1
#define i2cU5_0x1F		0x00A2
#define i2cU5_0x20		0x00A3
#define i2cU6_0x11		0x00A4	// 164 dec
#define i2cU6_0x12		0x00A5
#define i2cU6_0x13		0x00A6
#define i2cU6_0x14		0x00A7
#define i2cU6_0x15		0x00A8
#define i2cU6_0x16		0x00A9
#define i2cU6_0x17		0x00AA
#define i2cU6_0x18		0x00AB
#define i2cU6_0x19		0x00AC
#define i2cU6_0x1A		0x00AD
#define i2cU6_0x1B		0x00AE
#define i2cU6_0x1C		0x00AF
#define i2cU6_0x1D		0x00B0
#define i2cU6_0x1E		0x00B1
#define i2cU6_0x1F		0x00B2
#define i2cU6_0x20		0x00B3
#define i2cU7_0x11		0x00B4	// 180 dec
#define i2cU7_0x12		0x00B5
#define i2cU7_0x13		0x00B6
#define i2cU7_0x14		0x00B7
#define i2cU7_0x15		0x00B8
#define i2cU7_0x16		0x00B9
#define i2cU7_0x17		0x00BA
#define i2cU7_0x18		0x00BB
#define i2cU7_0x19		0x00BC
#define i2cU7_0x1A		0x00BD
#define i2cU7_0x1B		0x00BE
#define i2cU7_0x1C		0x00BF
#define i2cU7_0x1D		0x00C0
#define i2cU7_0x1E		0x00C1
#define i2cU7_0x1F		0x00C2
#define i2cU7_0x20		0x00C3	// 195 dec
#define TEMP_DHT11_1	0x0100	// 256 dec
#define TEMP_DHT11_2	0x0101
#define TEMP_DHT11_3	0x0102
#define TEMP_DHT11_4	0x0103
#define TEMP_DHT11_5	0x0104
#define TEMP_DHT11_6	0x0105
#define TEMP_DHT11_7	0x0106
#define TEMP_DHT11_8	0x0107
#define TEMP_DHT11_9	0x0108
#define TEMP_DHT11_10	0x0109
#define TEMP_DHT11_11	0x010A
#define TEMP_DHT11_12	0x010B
#define TEMP_DHT11_13	0x010C
#define TEMP_DHT11_14	0x010D
#define HUM_DHT11_1		0x010E
#define HUM_DHT11_2		0x010F
#define HUM_DHT11_3		0x0110
#define HUM_DHT11_4		0x0111
#define HUM_DHT11_5		0x0112
#define HUM_DHT11_6		0x0113
#define HUM_DHT11_7		0x0114
#define HUM_DHT11_8		0x0115
#define HUM_DHT11_9		0x0116
#define HUM_DHT11_10	0x0117
#define HUM_DHT11_11	0x0118
#define HUM_DHT11_12	0x0119
#define HUM_DHT11_13	0x011A
#define HUM_DHT11_14	0x011B
#define TEMP_DHT22_1	0x011C
#define TEMP_DHT22_2	0x011D
#define TEMP_DHT22_3	0x011E
#define TEMP_DHT22_4	0x011F
#define TEMP_DHT22_5	0x0120
#define TEMP_DHT22_6	0x0121
#define TEMP_DHT22_7	0x0122
#define TEMP_DHT22_8	0x0123
#define TEMP_DHT22_9	0x0124
#define TEMP_DHT22_10	0x0125
#define TEMP_DHT22_11	0x0126
#define TEMP_DHT22_12	0x0127
#define TEMP_DHT22_13	0x0128
#define TEMP_DHT22_14	0x0129
#define HUM_DHT22_1		0x012A
#define HUM_DHT22_2		0x012B
#define HUM_DHT22_3		0x012C
#define HUM_DHT22_4		0x012D
#define HUM_DHT22_5		0x012E
#define HUM_DHT22_6		0x012F
#define HUM_DHT22_7		0x0130
#define HUM_DHT22_8		0x0131
#define HUM_DHT22_9		0x0132
#define HUM_DHT22_10	0x0133
#define HUM_DHT22_11	0x0134
#define HUM_DHT22_12	0x0135
#define HUM_DHT22_13	0x0136
#define HUM_DHT22_14	0x0137	// 311 dec
#define HCSR04_1		0x0200	// 512 dec
#define HCSR04_2		0x0201	// 513 dec
#define HCSR04_3		0x0202	// 514 dec
#define HCSR04_4		0x0203	// 515 dec

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
#define BSL				0xEE
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
#define DHTXX_TEMP		0x0002
#define DHTXX_HUM		0x0003

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
#define USART_INVRTS	0x00400000 // USART RTS inverted mode
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
#define MCP3424_GAIN1	0x0000
#define MCP3424_GAIN2	0x0001
#define MCP3424_GAIN4	0x0002
#define MCP3424_GAIN8	0x0003

/// Software PWM registers
#define SPPCTL			0x00
#define SPPDUT			0x01
#define SPPPER			0x02
#define SPPVER			0x03

/// Masks for bits of control register
#define SPWM_ENABLE		0x8000	// SPWM enable

/// Default software PWM period
#define DEF_SPWM_PER	0x00C8	// Default software PWM period (for servo motors), 50 Hz (20 ms)

/// Ranges of servo motors PWM duty
#define MIN_SERV_DUTY	0x0007
#define MAX_SERV_DUTY	0x0017

#define MAX_STRING_LENGTH	0x20

#define WRITE_FUNC		0x03
#define READ_FUNC		0x05

/// Error defines
#define NO_ERROR		0x00
#define FUNC_CODE_ERROR	0x01
#define REG_ADDR_ERROR	0x02
#define REG_VAL_ERROR	0x03
#define SLAVE_ERROR		0x04
#define SLAVE_BUSY		0x06
#define DEV_ADDR_ERROR	0x11
#define CRC_ERROR		0x12
#define START_ERROR		0x13
#define LENGTH_ERROR	0x14
#define REG_INC_ERROR	0x15
#define UNDEF_ERROR		0xFF
#define URM04_ERROR		0xFFFFFFFF

/// HEX number sizes
#define NUM_BYTE		0x02
#define NUM_WORD		0x04
#define NUM_DWORD		0x08

#define RECV_PACK_LEN	0x12

/// Errors for response
#define NO_ERROR		0x00
#define DEVICE_ERROR	0x01
#define PACKET_ERROR	0x02

#define TIME_OUT		0xFFFF

/// Alternative functions of devices
#define ALT_NOTHING		0x00
#define ALT_ANALOG		0x01
#define ALT_I2C			0x02
#define ALT_SERVO		0x03
#define ALT_ENC			0x04
#define ALT_USART		0x05
#define ALT_DHTXX		0x06

/// USB device file
#define USB_DEV_NAME		"/dev/ttyACM0"

