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
 *  Created on: Fabruary 12, 2015
 *	Author: Rostislav Varzar
*/

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <termios.h>

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QThread>

#include <QsLog.h>

#include "usbMSP430Defines.h"
#include "usbMSP430Interface.h"

volatile uint16_t mper;			// Global PWM motor period
volatile uint16_t sper;			// Global software PWM period
int usb_out_descr;			// Input/Output USB device descriptor
struct termios usb_tty;			// Struct for termio parameters, MUST BE GLOBAL!!!
volatile uint8_t alt_func_flag;		// Alternate function switch flag for devices
uint8_t addr_table_i2c_usb[84] =	// Correspondence address table (between I2C and USB device addresses)
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, MOTOR1, MOTOR2, MOTOR3, MOTOR4,
		MOTOR1, MOTOR2, MOTOR4, MOTOR3, 0, 0, 0, 0, 0, 0,
		0, 0, SENSOR6, SENSOR5, SENSOR4, SENSOR3, SENSOR2, SENSOR1, SENSOR17, 0,
		0, 0, 0, 0, 0, 0, 0, 0, ENCODER1, ENCODER2,
		ENCODER3, ENCODER4, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, SPWM1, SPWM2, SPWM3, SPWM4, SPWM5, SPWM6, SPWM7,
		SPWM8, SPWM9, SPWM10, SPWM11, SPWM12, SPWM13, SPWM14, I2C1, I2C2, I2C3,
		I2C4, I2C5, I2C6, I2C7};

/// Delays class
class Sleeper : public QThread
{
	Q_OBJECT
public:
	static void usleep(unsigned long usecs){QThread::usleep(usecs);}
	static void msleep(unsigned long msecs){QThread::msleep(msecs);}
	static void sleep(unsigned long secs){QThread::sleep(secs);}
};

/// Extract number from packet
uint32_t hex2num(char *string
		, uint16_t pos
		, uint16_t numsize)
{
	uint32_t resnum = 0;
	uint32_t tmpnum = 0;
	char c = 0;
	for (uint16_t i = 0; i < numsize; i++)
	{
		c = toupper(string[pos+i]);
		tmpnum = c > '9' ? c - 'A' + 10 : c - '0';
		resnum |= (tmpnum << ((numsize - i - 1) * 4));
	}
	return resnum;
}

/// Make write register packet
void makeWriteRegPacket(char *msp_packet
			, uint8_t dev_addr
			, uint8_t reg_addr
			, uint32_t reg_val)
{
	uint8_t crc = (0xFF - (dev_addr + WRITE_FUNC + reg_addr + uint8_t(reg_val & 0xFF)
					+ uint8_t((reg_val >> 8) & 0xFF)
					+ uint8_t((reg_val >> 16) & 0xFF)
					+ uint8_t((reg_val >> 24) & 0xFF)) + 1) & 0xFF;			// Checksum
	sprintf(msp_packet, ":%02X%02X%02X%08X%02X\n", dev_addr, WRITE_FUNC, reg_addr, reg_val, crc);
}

/// Make read register packet
void makeReadRegPacket(char *msp_packet
			, uint8_t dev_addr
			, uint8_t reg_addr)
{
	uint8_t crc = (0xFF - (dev_addr + READ_FUNC + reg_addr) + 1) & 0xFF;	// Checksum
	sprintf(msp_packet, ":%02X%02X%02X%02X\n", dev_addr, READ_FUNC, reg_addr, crc);
}

/// Function for decoding received packet
uint32_t decodeReceivedPacket(char *msp_packet
				, uint8_t &dev_addr
				, uint8_t &func_code
				, uint8_t &reg_addr
				, uint32_t &reg_val)
{
	uint8_t crc1 = 0;				// Received cheksum
	uint8_t crc2 = 0;				// Calculated checksum
	if (msp_packet[0] != ':')			// Start condition error
	{
		return START_ERROR;
	}
	if ((strlen(msp_packet) != RECV_PACK_LEN))	// Incorrect packet length
	{
		return LENGTH_ERROR;
	}
	dev_addr = hex2num(msp_packet, 1, NUM_BYTE);	// Get device address
	func_code = hex2num(msp_packet, 3, NUM_BYTE);	// Get function
	reg_addr = hex2num(msp_packet, 5, NUM_BYTE);	// Get register address
	reg_val = hex2num(msp_packet, 7, NUM_DWORD);	// Get register value
	crc1 = hex2num(msp_packet, 15, NUM_BYTE);	// Get CRC from packet
	crc2 = (0xFF
					- (dev_addr + func_code + reg_addr + uint8_t(reg_val & 0xFF)
							+ uint8_t((reg_val >> 8) & 0xFF)
							+ uint8_t((reg_val >> 16) & 0xFF)
							+ uint8_t((reg_val >> 24) & 0xFF))
					+ 1) & 0xFF;			// Calculate CRC
	if (crc1 != crc2)				// Check CRC
		return CRC_ERROR;
	return NO_ERROR;				// Return NO ERROR
}

/// Init USB TTY device
uint32_t init_USBTTYDevice()
{
	memset (&usb_tty, 0, sizeof(usb_tty));

	if (tcgetattr(usb_out_descr, &usb_tty) != 0)
	{
		QLOG_ERROR() << "Error " << errno << " from tcgetattr: " << strerror(errno);
		return DEVICE_ERROR;
	}

	cfsetospeed (&usb_tty, __MAX_BAUD);
	cfsetispeed (&usb_tty, __MAX_BAUD);

	usb_tty.c_cflag     &=  ~PARENB;			    // Make 8n1
	usb_tty.c_cflag     &=  ~CSTOPB;
	usb_tty.c_cflag     &=  ~CSIZE;
	usb_tty.c_cflag     |=  CS8;
	usb_tty.c_cflag     &=  ~CRTSCTS;			    // No flow control
	usb_tty.c_lflag     =   0;				    // No signaling chars, no echo, no canonical processing
	usb_tty.c_oflag     =   0;				    // No remapping, no delays
	usb_tty.c_cc[VMIN]      =   0;				    // Read doesn't block
	usb_tty.c_cc[VTIME]     =   1;				    // 0.1 seconds read timeout
	usb_tty.c_cflag     |=  CREAD | CLOCAL;			    // Turn on READ & ignore ctrl lines
	usb_tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);	    // Turn off s/w flow ctrl
	usb_tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG);    // Make raw
	usb_tty.c_oflag     &=  ~OPOST;				    // Make raw

	tcflush(usb_out_descr, TCIFLUSH);

	if (tcsetattr(usb_out_descr, TCSANOW, &usb_tty) != 0)
	{
		QLOG_ERROR() << "Error " << errno << " from tcsetattr";
		return DEVICE_ERROR;
	}

	return NO_ERROR;
}

/// Send USB packet
uint32_t sendUSBPacket(char *in_msp_packet
			, char *out_msp_packet)
{
	uint32_t tout = 0;	    // Timeout counter
	uint32_t n_written = 0;	    // Number of written bytes
	int32_t n_read = 0;	    // Number of read bytes
	size_t n_write = 0;	    // Number to write bytes
	char s1[MAX_STRING_LENGTH]; // Temp string

	if (usb_out_descr < 0)
	{
		QLOG_ERROR() << "Error device descriptor" << errno << " : " << strerror (errno);
		return DEVICE_ERROR;
	}

	// Write packet
	n_write = strlen(in_msp_packet);
	strcpy(s1, in_msp_packet);
	n_written = write(usb_out_descr, s1, n_write);
	if (n_written != strlen(s1))
	{
		QLOG_ERROR() << "Error writing: " << strerror(errno);
		return PACKET_ERROR;
	}
	tcflush(usb_out_descr, TCOFLUSH);


	// Read packet
	memset (&s1, '\0', MAX_STRING_LENGTH);
	do
	{
		n_read = read(usb_out_descr, &s1, MAX_STRING_LENGTH);
		tout ++;
	} while ((n_read < RECV_PACK_LEN) && (tout < TIME_OUT));
	tcflush(usb_out_descr, TCIFLUSH);

	if ((n_read != RECV_PACK_LEN) || (tout == TIME_OUT))
	{
		QLOG_ERROR() << "Error reading: " << strerror(errno);
		out_msp_packet[0] = 0x00;
		out_msp_packet[1] = 0x00;
		return PACKET_ERROR;
	}
	else
	{
		memcpy(out_msp_packet, s1, RECV_PACK_LEN);
		out_msp_packet[RECV_PACK_LEN] = 0;
	}

	return NO_ERROR;
}

/// Init power motors
uint32_t init_motors_USBMSP()
{
	char s1[MAX_STRING_LENGTH];	// Temp string

	if (DEF_MOT_PER > 0)
	{
		mper = (uint16_t)(24 / 8 * (float)DEF_MOT_PER);
		if (mper < 1)
			mper = 1;
	}
	else
	{
		mper = 1;
	}
	makeWriteRegPacket(s1, MOTOR1, MMPER, mper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, MOTOR2, MMPER, mper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, MOTOR3, MMPER, mper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, MOTOR4, MMPER, mper);
	sendUSBPacket(s1, s1);
	return NO_ERROR;
}

/// Init servo motors
uint32_t init_servomotors_USBMSP()
{
	char s1[MAX_STRING_LENGTH];	// Temp string

	if (DEF_SPWM_PER > 0)
	{
		sper = DEF_SPWM_PER;
	}
	else
	{
		sper = 1;
	}
	makeWriteRegPacket(s1, SPWM1, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM2, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM3, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM4, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM5, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM6, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM7, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM8, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM9, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM10, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM11, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM12, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM13, SPPPER, sper);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SPWM14, SPPPER, sper);
	sendUSBPacket(s1, s1);
	return NO_ERROR;
}

/// Init encoders
uint32_t init_encoders_USBMSP()
{
	char s1[MAX_STRING_LENGTH];	// Temp string

	makeWriteRegPacket(s1, ENCODER1, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, ENCODER2, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, ENCODER3, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, ENCODER4, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
	sendUSBPacket(s1, s1);
	return NO_ERROR;
}

/// Init sensors
uint32_t init_sensors_USBMSP()
{
	char s1[MAX_STRING_LENGTH];	// Temp string

	makeWriteRegPacket(s1, SENSOR1, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR1, SSIDX, ANALOG_INP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR2, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR2, SSIDX, ANALOG_INP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR3, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR3, SSIDX, ANALOG_INP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR4, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR4, SSIDX, ANALOG_INP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR5, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR5, SSIDX, ANALOG_INP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR6, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR6, SSIDX, ANALOG_INP);
	sendUSBPacket(s1, s1);
	// Battery
	makeWriteRegPacket(s1, SENSOR17, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR17, SSIDX, ANALOG_INP);
	sendUSBPacket(s1, s1);
	return NO_ERROR;
}

/// Init DHTxx sensors
uint32_t init_dhtxx_sensors_USBMSP()
{
	char s1[MAX_STRING_LENGTH];	// Temp string

	makeWriteRegPacket(s1, SENSOR7, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR7, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR8, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR8, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR9, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR9, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR10, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR10, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR11, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR11, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR12, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR12, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR13, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR13, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR14, SSCTL, SENS_ENABLE + SENS_READ);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, SENSOR14, SSIDX, DHTXX_TEMP);
	sendUSBPacket(s1, s1);

	return NO_ERROR;
}

/// Init I2C sensors
uint32_t init_i2c_sensors_USBMSP()
{
	char s1[MAX_STRING_LENGTH];	// Temp string

	makeWriteRegPacket(s1, I2C1, IIIDX, NXTTEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C1, IICTL, I2C_ENABLE + I2C_SENS);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C2, IIIDX, NXTTEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C2, IICTL, I2C_ENABLE + I2C_SENS);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C3, IIIDX, NXTTEMP);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C3, IICTL, I2C_ENABLE + I2C_SENS);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C4, IIIDX, MCP3424_CH1);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C4, IIPAR, MCP3424_GAIN1);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C4, IICTL, I2C_ENABLE + I2C_SENS);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C5, IIIDX, MCP3424_CH1);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C5, IIPAR, MCP3424_GAIN2);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C5, IICTL, I2C_ENABLE + I2C_SENS);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C6, IIIDX, MCP3424_CH1);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C6, IIPAR, MCP3424_GAIN4);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C6, IICTL, I2C_ENABLE + I2C_SENS);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C7, IIIDX, MCP3424_CH1);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C7, IIPAR, MCP3424_GAIN8);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, I2C7, IICTL, I2C_ENABLE + I2C_SENS);
	sendUSBPacket(s1, s1);
	return NO_ERROR;
}

/// Connect to USB MSP430 device
uint32_t connect_USBMSP()
{
	// Open USB descriptor for writing
	usb_out_descr = open(USB_DEV_NAME, O_RDWR | O_NONBLOCK | O_NDELAY | O_CLOEXEC);
	if (usb_out_descr < 0)
	{
		QLOG_INFO() << "Error " << errno << " opening " << USB_DEV_NAME << ": " << strerror (errno);
		return DEVICE_ERROR;
	}

	// Init USB STTY device with serial port parameters
	init_USBTTYDevice();

	// Init servo motors
	init_servomotors_USBMSP();

	// Init I2C sensors
	init_i2c_sensors_USBMSP();

	// Init DHTxx sensors
	init_dhtxx_sensors_USBMSP();

	// Init motors
	init_motors_USBMSP();

	// Init encoders
	init_encoders_USBMSP();

	// Init sensors
	init_sensors_USBMSP();

	// Default alternative function
	alt_func_flag = ALT_ANALOG;

	return NO_ERROR;
}

/// Disconnect from USB MSP430 device
uint32_t disconnect_USBMSP()
{
	if (usb_out_descr < 0)
	{
		QLOG_ERROR() << "Error device descriptor" << errno << " : " << strerror (errno);
		return DEVICE_ERROR;
	}
	close(usb_out_descr);

	return NO_ERROR;
}

/// Motor power control function
uint32_t power_Motor(QByteArray const &i2c_data)
{
	uint16_t mdut;				    // Motor PWM duty
	uint16_t mctl;				    // Motor control register
	uint16_t sdut;				    // Software PWM duty
	uint16_t sctl;				    // Software PWM control register
	int8_t mtmp;				    // Temp variable
	char s1[MAX_STRING_LENGTH];		    // Temp string variable
	const int8_t reg_value = i2c_data[2];	    // Register value
	const uint16_t dev_address = (uint16_t)i2c_data[0] +
		((uint16_t)i2c_data[1] << 8);	    // Device address

	// Power motors
	if ((dev_address == i2cMOT1) || (dev_address == i2cMOT2) ||
			(dev_address == i2cMOT3) || (dev_address == i2cMOT4))
	{

		// qDebug() << "Dev address (power_motor): " << dev_address << " " << reg_value;
		mtmp = reg_value;
		mctl = MOT_ENABLE;
		if ((mtmp < -100) || (mtmp > 100))
		{
			mctl = mctl + MOT_BRAKE;
			mtmp = 0;
		}
		if (mtmp < 0)
			mctl = mctl + MOT_BACK;
		if (mtmp != 0)
			mctl = mctl + MOT_POWER;
		if (mtmp < -100)
			mtmp = -100;
		if (mtmp > 100)
			mtmp = 100;
		mdut = uint16_t(float(abs(mtmp)) * (mper - 1) / 100);

		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], MMDUT, mdut);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], MMCTL, mctl);
		sendUSBPacket(s1, s1);
	}
	// Servo motors
	else if ((dev_address == i2cSERV1) || (dev_address == i2cSERV2) ||
		 (dev_address == i2cSERV3) || (dev_address == i2cSERV4) ||
		 (dev_address == i2cSERV5) || (dev_address == i2cSERV6) ||
		 (dev_address == i2cSERV7) || (dev_address == i2cSERV8) ||
		 (dev_address == i2cSERV9) || (dev_address == i2cSERV10) ||
		 (dev_address == i2cSERV11) || (dev_address == i2cSERV12) ||
		 (dev_address == i2cSERV13) || (dev_address == i2cSERV14))
	{

		// qDebug() << "Dev address (servo_motor): " << dev_address << " " << reg_value;
		mtmp = reg_value;
		if ((mtmp == INT8_MIN) || (mtmp == INT8_MAX))
			mtmp = 0;
		if (mtmp < -100)
			mtmp = -100;
		if (mtmp > 100)
			mtmp = 100;
		// sdut = uint16_t((float(mtmp) + 100) / 200 * (MAX_SERV_DUTY - MIN_SERV_DUTY) + 7);
		sdut = uint16_t((float(abs(mtmp))) / 100 * (MAX_SERV_DUTY - MIN_SERV_DUTY) + 7);
		if (mtmp != 0) {
			sctl = SPWM_ENABLE;
		} else {
			sctl = 0;
			sdut = 0;
		}

		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_ANALOG)
				|| (alt_func_flag == ALT_ENC)
				|| (alt_func_flag == ALT_I2C)
				|| (alt_func_flag == ALT_USART)
				|| (alt_func_flag == ALT_DHTXX))
		{
		}

		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], SPPPER, sper);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], SPPDUT, sdut);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], SPPCTL, sctl);
		sendUSBPacket(s1, s1);
		alt_func_flag = ALT_SERVO;
	}
	else
	{
		return DEV_ADDR_ERROR;
	}

	return NO_ERROR;
}

/// Set motor frequency function
uint32_t freq_Motor(QByteArray const &i2c_data)
{
	char s1[MAX_STRING_LENGTH];					// Temp string variable
	const uint16_t reg_value = ((i2c_data[3] << 8) | i2c_data[2]);	// Register value
	const uint16_t dev_address = (uint16_t)i2c_data[0] +
		((uint16_t)i2c_data[1] << 8);	    // Device address

	if ((dev_address == i2cPWMMOT1) || (dev_address == i2cPWMMOT2) ||
			(dev_address == i2cPWMMOT3) || (dev_address == i2cPWMMOT4))
	{
		// qDebug() << "Dev address (freq_motor): " << dev_address << " " << reg_value;
		if (reg_value > 0)
		{
			mper = (uint16_t)(24 / 8 * (float)reg_value);
			if (mper < 1)
				mper = 1;
		}
		else
		{
			mper = 1;
		}
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], MMPER, mper);
		sendUSBPacket(s1, s1);
	}
	else
	{
		return DEV_ADDR_ERROR;
	}

	return NO_ERROR;
}

/// Reset encoder function
uint32_t reset_Encoder(QByteArray const &i2c_data)
{
	char s1[MAX_STRING_LENGTH];		    // Temp string variable
	const uint8_t reg_value = i2c_data[2];	    // Register value
	const uint16_t dev_address = (uint16_t)i2c_data[0] +
		((uint16_t)i2c_data[1] << 8);	    // Device address

	if ((dev_address == i2cENC1) || (dev_address == i2cENC2)
		||	(dev_address == i2cENC3) || (dev_address == i2cENC4))
	{
		// qDebug() << "Dev address (reset_encoder): " << dev_address << " " << reg_value;
		if ((alt_func_flag == ALT_NOTHING) || (alt_func_flag == ALT_SERVO)
			|| (alt_func_flag == ALT_I2C) || (alt_func_flag == ALT_USART)
			|| (alt_func_flag == ALT_DHTXX) || (alt_func_flag == ALT_ANALOG))
		{
		}
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address],
				   EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], EEVAL, reg_value);
		sendUSBPacket(s1, s1);
		alt_func_flag = ALT_ENC;
	}
	else
	{
		return DEV_ADDR_ERROR;
	}

	return NO_ERROR;
}

/// Read encoder function
uint32_t read_Encoder(QByteArray const &i2c_data)
{
	char s1[MAX_STRING_LENGTH] {};		    // Temp string variable
	char s2[MAX_STRING_LENGTH] {};		    // Temp string variable
	uint32_t errcode;			    // Returned error code
	Q_UNUSED(errcode);
	uint8_t devaddr;			    // Returned device address
	uint8_t funccode;			    // Returned function code
	uint8_t regaddr;			    // Returned register address
	uint32_t regval=UINT32_MAX;		    // Returned register value
	uint16_t tmout = 0;			    // Reading timeout
	const uint16_t dev_address = (uint16_t)i2c_data[0] +
		((uint16_t)i2c_data[1] << 8);	    // Device address

	if ((dev_address == i2cENC1) || (dev_address == i2cENC2)
		||  (dev_address == i2cENC3) || (dev_address == i2cENC4))
	{
		if ((alt_func_flag == ALT_NOTHING) || (alt_func_flag == ALT_SERVO)
			|| (alt_func_flag == ALT_I2C) || (alt_func_flag == ALT_USART)
			|| (alt_func_flag == ALT_DHTXX) || (alt_func_flag == ALT_ANALOG))
		{
		}
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address],
					EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
		sendUSBPacket(s1, s1);
		do
		{
			makeReadRegPacket(s1, addr_table_i2c_usb[dev_address], EEVAL);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != addr_table_i2c_usb[dev_address]) || (regaddr != EEVAL)) && (tmout < TIME_OUT));
		alt_func_flag = ALT_ENC;
		// qDebug() << "Dev address (read_encoder): " << dev_address << " " << regval;
		return regval;
	}
	else
	{
		return DEV_ADDR_ERROR;
	}

	return NO_ERROR;
}

/// Init I2C + USART + URM04
uint32_t init_URM04(uint8_t i2c_addr, uint8_t usart_addr)
{
	char s1[MAX_STRING_LENGTH];		    // Temp string variable
	makeWriteRegPacket(s1, i2c_addr, IICTL, I2C_ENABLE);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, usart_addr, UUSPD, 19200);
	sendUSBPacket(s1, s1);
	makeWriteRegPacket(s1, usart_addr, UUCTL, USART_EN+USART_8BITS+USART_RS485+USART_INVRTS+USART_RXEN+USART_TXEN);
	sendUSBPacket(s1, s1);
	return NO_ERROR;
}

/// Read URM04 distance function
uint32_t read_URM04_dist(uint8_t dev_addr, uint8_t urm04_addr)
{
	char s1[MAX_STRING_LENGTH];		    // Temp string variable
	char s2[MAX_STRING_LENGTH];		    // Temp string variable
	uint8_t crc1;			    // URM04 CRC
	uint32_t errcode;			    // Returned error code
	Q_UNUSED(errcode);
	uint8_t devaddr;			    // Returned device address
	uint8_t funccode;			    // Returned function code
	uint8_t regaddr;			    // Returned register address
	uint32_t regval=UINT32_MAX;		    // Returned register value
	uint16_t tmout = 0;			    // Reading timeout
	uint8_t crc_a = 0x55 + 0xAA + urm04_addr + 0x00 + 0x01;
	uint8_t crc_b = 0x55 + 0xAA + urm04_addr + 0x00 + 0x02;
	uint8_t pack1[6] = {0x55, 0xAA, urm04_addr, 0x00, 0x01, crc_a};
	uint8_t pack2[6] = {0x55, 0xAA, urm04_addr, 0x00, 0x02, crc_b};
	uint8_t buf1[8] = {0};
	// Trigger device
	for (auto x : pack1)
	{
		makeWriteRegPacket(s1, dev_addr, UUDAT, x);
		sendUSBPacket(s1, s1);
	}
	// Wait about 400 ms
	Sleeper::msleep(400);
	// Read distance
	for (auto x: pack2)
	{
		makeWriteRegPacket(s1, dev_addr, UUDAT, x);
		sendUSBPacket(s1, s1);
	}
	for (auto &x: buf1)
	{
		do
		{
			makeReadRegPacket(s1, dev_addr, UUDAT);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != dev_addr) || (regaddr != UUDAT)) && (tmout < TIME_OUT));
		x = regval;
	}
	crc1 = buf1[0] + buf1[1] + buf1[2] + buf1[3] + buf1[4] + buf1[5] + buf1[6];
	if (crc1 != buf1[7])
	{
		// qDebug() << "URM04 CRC ERROR!";
		return URM04_ERROR;
	}
	else
	{
		// qDebug() << "Distance: " << (uint32_t)(buf1[5] << 8) + (uint32_t)buf1[6];
		return (uint32_t)(buf1[5] << 8) + (uint32_t)buf1[6];
	}
}

/// Read sensor function
uint32_t read_Sensor(QByteArray const &i2c_data)
{
	char s1[MAX_STRING_LENGTH];		    // Temp string variable
	char s2[MAX_STRING_LENGTH];		    // Temp string variable
	uint32_t errcode;			    // Returned error code
	Q_UNUSED(errcode);
	uint8_t devaddr;			    // Returned device address
	uint8_t funccode;			    // Returned function code
	uint8_t regaddr;			    // Returned register address
	uint32_t regval=UINT32_MAX;		    // Returned register value
	uint16_t tmout = 0;			    // Reading timeout
	const uint16_t dev_address = (uint16_t)i2c_data[0] +
		((uint16_t)i2c_data[1] << 8);	    // Device address

	// Analog sensors
	if ((dev_address == i2cSENS1)
			|| (dev_address == i2cSENS2)
			|| (dev_address == i2cSENS3)
			|| (dev_address == i2cSENS4)
			|| (dev_address == i2cSENS5)
			|| (dev_address == i2cSENS6)
			|| (dev_address == i2cBATT))
	{
		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_SERVO)
				|| (alt_func_flag == ALT_I2C)
				|| (alt_func_flag == ALT_USART)
				|| (alt_func_flag == ALT_DHTXX))
		{
		}

		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], SSCTL, SENS_ENABLE + SENS_READ);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], SSIDX, ANALOG_INP);
		sendUSBPacket(s1, s1);
		do
		{
			makeReadRegPacket(s1, addr_table_i2c_usb[dev_address], SSVAL);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != addr_table_i2c_usb[dev_address]) || (regaddr != SSVAL)) && (tmout < TIME_OUT));

		alt_func_flag = ALT_ANALOG;
		// qDebug() << "Dev address (analog_sensor): " << dev_address << " " << regval;
		// qDebug() << "Dev address (analog_sensor): " << addr_table_i2c_usb[dev_address] << " " << regval;
		return regval;
	}
	// I2C sensors
	else if ((dev_address == i2cTEMP1)
			|| (dev_address == i2cTEMP2)
			|| (dev_address == i2cTEMP3)
			|| (dev_address == i2cW1)
			|| (dev_address == i2cW2)
			|| (dev_address == i2cW3)
			|| (dev_address == i2cW4))
	{
		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_SERVO)
				|| (alt_func_flag == ALT_ANALOG)
				|| (alt_func_flag == ALT_ENC)
				|| (alt_func_flag == ALT_USART)
				|| (alt_func_flag == ALT_DHTXX))
		{
		}

		makeWriteRegPacket(s1, addr_table_i2c_usb[dev_address], IICTL, I2C_ENABLE + I2C_SENS);
		sendUSBPacket(s1, s1);
		do
		{
			makeReadRegPacket(s1, addr_table_i2c_usb[dev_address], IIVAL);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != addr_table_i2c_usb[dev_address]) || (regaddr != IIVAL)) && (tmout < TIME_OUT));

		alt_func_flag = ALT_I2C;
		// qDebug() << "Dev address (i2c_sensor): " << dev_address << " " << regval;
		return regval;
	}
	// DHT11 sensors (temperature)
	else if ((dev_address >= TEMP_DHT11_1) && (dev_address <= TEMP_DHT11_14))
	{
		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_SERVO)
				|| (alt_func_flag == ALT_ANALOG)
				|| (alt_func_flag == ALT_ENC)
				|| (alt_func_flag == ALT_USART)
				|| (alt_func_flag == ALT_I2C))
		{
		}

		makeWriteRegPacket(s1, (dev_address-TEMP_DHT11_1+SENSOR1), SSCTL, SENS_ENABLE + SENS_READ);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, (dev_address-TEMP_DHT11_1+SENSOR1), SSIDX, DHTXX_TEMP);
		sendUSBPacket(s1, s1);

		do
		{
			makeReadRegPacket(s1, (dev_address-TEMP_DHT11_1+SENSOR1), SSVAL);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != (dev_address-TEMP_DHT11_1+SENSOR1)) || (regaddr != SSVAL)) && (tmout < TIME_OUT));

		alt_func_flag = ALT_DHTXX;
		// qDebug() << "Dev address (dht11_temperature): " << dev_address << " " << regval;
		// qDebug() << "Dev address (dht11_temperature): " << (dev_address-TEMP_DHT11_1+SENSOR1) << " " << regval;
		return regval;
	}
	// DHT11 sensors (humidity)
	else if ((dev_address >= HUM_DHT11_1) && (dev_address <= HUM_DHT11_14))
	{
		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_SERVO)
				|| (alt_func_flag == ALT_ANALOG)
				|| (alt_func_flag == ALT_ENC)
				|| (alt_func_flag == ALT_USART)
				|| (alt_func_flag == ALT_I2C))
		{
		}

		makeWriteRegPacket(s1, (dev_address-HUM_DHT11_1+SENSOR1), SSCTL, SENS_ENABLE + SENS_READ);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, (dev_address-HUM_DHT11_1+SENSOR1), SSIDX, DHTXX_HUM);
		sendUSBPacket(s1, s1);

		do
		{
			makeReadRegPacket(s1, (dev_address-HUM_DHT11_1+SENSOR1), SSVAL);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != (dev_address-HUM_DHT11_1+SENSOR1)) || (regaddr != SSVAL)) && (tmout < TIME_OUT));
		alt_func_flag = ALT_DHTXX;
		// qDebug() << "Dev address (dht11_humidity): " << dev_address << " " << regval;
		// qDebug() << "Dev address (dht11_humidity): " << (dev_address-HUM_DHT11_1+SENSOR1) << " " << regval;
		return regval;
	}
	// DHT22 sensors (temperature)
	else if ((dev_address >= TEMP_DHT22_1) && (dev_address <= TEMP_DHT22_14))
	{
		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_SERVO)
				|| (alt_func_flag == ALT_ANALOG)
				|| (alt_func_flag == ALT_ENC)
				|| (alt_func_flag == ALT_USART)
				|| (alt_func_flag == ALT_I2C))
		{
		}

		makeWriteRegPacket(s1, (dev_address-TEMP_DHT22_1+SENSOR1), SSCTL, SENS_ENABLE + SENS_READ);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, (dev_address-TEMP_DHT22_1+SENSOR1), SSIDX, DHTXX_TEMP);
		sendUSBPacket(s1, s1);

		do
		{
			makeReadRegPacket(s1, (dev_address-TEMP_DHT22_1+SENSOR1), SSVAL);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != (dev_address-TEMP_DHT22_1+SENSOR1)) || (regaddr != SSVAL)) && (tmout < TIME_OUT));

		alt_func_flag = ALT_DHTXX;
		// qDebug() << "Dev address (dht22_temperature): " << dev_address << " " << regval;
		// qDebug() << "Dev address (dht22_temperature): " << (dev_address-TEMP_DHT22_1+SENSOR1) << " " << regval;
		return regval;
	}
	// DHT22 sensors (humidity)
	else if ((dev_address >= HUM_DHT22_1) && (dev_address <= HUM_DHT22_14))
	{
		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_SERVO)
				|| (alt_func_flag == ALT_ANALOG)
				|| (alt_func_flag == ALT_ENC)
				|| (alt_func_flag == ALT_USART)
				|| (alt_func_flag == ALT_I2C))
		{
		}

		makeWriteRegPacket(s1, (dev_address-HUM_DHT22_1+SENSOR1), SSCTL, SENS_ENABLE + SENS_READ);
		sendUSBPacket(s1, s1);
		makeWriteRegPacket(s1, (dev_address-HUM_DHT22_1+SENSOR1), SSIDX, DHTXX_HUM);
		sendUSBPacket(s1, s1);

		do
		{
			makeReadRegPacket(s1, (dev_address-HUM_DHT22_1+SENSOR1), SSVAL);
			errcode = sendUSBPacket(s1, s2);
			errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
			tmout ++;
		} while (((devaddr != (dev_address-HUM_DHT22_1+SENSOR1)) || (regaddr != SSVAL)) && (tmout < TIME_OUT));

		alt_func_flag = ALT_DHTXX;
		// qDebug() << "Dev address (dht22_humidity): " << dev_address << " " << regval;
		// qDebug() << "Dev address (dht22_humidity): " << (dev_address-HUM_DHT22_1+SENSOR1) << " " << regval;;;
		return regval;
	}
	// URM04 sensors
	else if ((dev_address >= i2cU1_0x11) && (dev_address <= i2cU7_0x20))
	{
		if ((alt_func_flag == ALT_NOTHING)
				|| (alt_func_flag == ALT_SERVO)
				|| (alt_func_flag == ALT_ANALOG)
				|| (alt_func_flag == ALT_ENC)
				|| (alt_func_flag == ALT_I2C)
				|| (alt_func_flag == ALT_DHTXX))
		{
		}

		if ((dev_address >= i2cU1_0x11) && (dev_address <= i2cU1_0x20))
		{
			init_URM04(I2C1, USART1);
		}
		if ((dev_address >= i2cU2_0x11) && (dev_address <= i2cU2_0x20))
		{
			init_URM04(I2C2, USART2);
		}
		if ((dev_address >= i2cU3_0x11) && (dev_address <= i2cU3_0x20))
		{
			init_URM04(I2C3, USART3);
		}
		if ((dev_address >= i2cU4_0x11) && (dev_address <= i2cU4_0x20))
		{
			init_URM04(I2C4, USART4);
		}
		if ((dev_address >= i2cU5_0x11) && (dev_address <= i2cU5_0x20))
		{
			init_URM04(I2C5, USART5);
		}
		if ((dev_address >= i2cU6_0x11) && (dev_address <= i2cU6_0x20))
		{
			init_URM04(I2C6, USART6);
		}
		if ((dev_address >= i2cU7_0x11) && (dev_address <= i2cU7_0x20))
		{
			init_URM04(I2C7, USART7);
		}
		alt_func_flag = ALT_USART;
		// qDebug() << "Dev address (URM04_sensor): " << dev_address << " " << regval;
		switch (dev_address)
		{
		case i2cU1_0x11: return read_URM04_dist(USART1, 0x11);
		case i2cU1_0x12: return read_URM04_dist(USART1, 0x12);
		case i2cU1_0x13: return read_URM04_dist(USART1, 0x13);
		case i2cU1_0x14: return read_URM04_dist(USART1, 0x14);
		case i2cU1_0x15: return read_URM04_dist(USART1, 0x15);
		case i2cU1_0x16: return read_URM04_dist(USART1, 0x16);
		case i2cU1_0x17: return read_URM04_dist(USART1, 0x17);
		case i2cU1_0x18: return read_URM04_dist(USART1, 0x18);
		case i2cU1_0x19: return read_URM04_dist(USART1, 0x19);
		case i2cU1_0x1A: return read_URM04_dist(USART1, 0x1A);
		case i2cU1_0x1B: return read_URM04_dist(USART1, 0x1B);
		case i2cU1_0x1C: return read_URM04_dist(USART1, 0x1C);
		case i2cU1_0x1D: return read_URM04_dist(USART1, 0x1D);
		case i2cU1_0x1E: return read_URM04_dist(USART1, 0x1E);
		case i2cU1_0x1F: return read_URM04_dist(USART1, 0x1F);
		case i2cU1_0x20: return read_URM04_dist(USART1, 0x20);
		case i2cU2_0x11: return read_URM04_dist(USART2, 0x11);
		case i2cU2_0x12: return read_URM04_dist(USART2, 0x12);
		case i2cU2_0x13: return read_URM04_dist(USART2, 0x13);
		case i2cU2_0x14: return read_URM04_dist(USART2, 0x14);
		case i2cU2_0x15: return read_URM04_dist(USART2, 0x15);
		case i2cU2_0x16: return read_URM04_dist(USART2, 0x16);
		case i2cU2_0x17: return read_URM04_dist(USART2, 0x17);
		case i2cU2_0x18: return read_URM04_dist(USART2, 0x18);
		case i2cU2_0x19: return read_URM04_dist(USART2, 0x19);
		case i2cU2_0x1A: return read_URM04_dist(USART2, 0x1A);
		case i2cU2_0x1B: return read_URM04_dist(USART2, 0x1B);
		case i2cU2_0x1C: return read_URM04_dist(USART2, 0x1C);
		case i2cU2_0x1D: return read_URM04_dist(USART2, 0x1D);
		case i2cU2_0x1E: return read_URM04_dist(USART2, 0x1E);
		case i2cU2_0x1F: return read_URM04_dist(USART2, 0x1F);
		case i2cU2_0x20: return read_URM04_dist(USART2, 0x20);
		case i2cU3_0x11: return read_URM04_dist(USART3, 0x11);
		case i2cU3_0x12: return read_URM04_dist(USART3, 0x12);
		case i2cU3_0x13: return read_URM04_dist(USART3, 0x13);
		case i2cU3_0x14: return read_URM04_dist(USART3, 0x14);
		case i2cU3_0x15: return read_URM04_dist(USART3, 0x15);
		case i2cU3_0x16: return read_URM04_dist(USART3, 0x16);
		case i2cU3_0x17: return read_URM04_dist(USART3, 0x17);
		case i2cU3_0x18: return read_URM04_dist(USART3, 0x18);
		case i2cU3_0x19: return read_URM04_dist(USART3, 0x19);
		case i2cU3_0x1A: return read_URM04_dist(USART3, 0x1A);
		case i2cU3_0x1B: return read_URM04_dist(USART3, 0x1B);
		case i2cU3_0x1C: return read_URM04_dist(USART3, 0x1C);
		case i2cU3_0x1D: return read_URM04_dist(USART3, 0x1D);
		case i2cU3_0x1E: return read_URM04_dist(USART3, 0x1E);
		case i2cU3_0x1F: return read_URM04_dist(USART3, 0x1F);
		case i2cU3_0x20: return read_URM04_dist(USART3, 0x20);
		case i2cU4_0x11: return read_URM04_dist(USART4, 0x11);
		case i2cU4_0x12: return read_URM04_dist(USART4, 0x12);
		case i2cU4_0x13: return read_URM04_dist(USART4, 0x13);
		case i2cU4_0x14: return read_URM04_dist(USART4, 0x14);
		case i2cU4_0x15: return read_URM04_dist(USART4, 0x15);
		case i2cU4_0x16: return read_URM04_dist(USART4, 0x16);
		case i2cU4_0x17: return read_URM04_dist(USART4, 0x17);
		case i2cU4_0x18: return read_URM04_dist(USART4, 0x18);
		case i2cU4_0x19: return read_URM04_dist(USART4, 0x19);
		case i2cU4_0x1A: return read_URM04_dist(USART4, 0x1A);
		case i2cU4_0x1B: return read_URM04_dist(USART4, 0x1B);
		case i2cU4_0x1C: return read_URM04_dist(USART4, 0x1C);
		case i2cU4_0x1D: return read_URM04_dist(USART4, 0x1D);
		case i2cU4_0x1E: return read_URM04_dist(USART4, 0x1E);
		case i2cU4_0x1F: return read_URM04_dist(USART4, 0x1F);
		case i2cU4_0x20: return read_URM04_dist(USART4, 0x20);
		case i2cU5_0x11: return read_URM04_dist(USART5, 0x11);
		case i2cU5_0x12: return read_URM04_dist(USART5, 0x12);
		case i2cU5_0x13: return read_URM04_dist(USART5, 0x13);
		case i2cU5_0x14: return read_URM04_dist(USART5, 0x14);
		case i2cU5_0x15: return read_URM04_dist(USART5, 0x15);
		case i2cU5_0x16: return read_URM04_dist(USART5, 0x16);
		case i2cU5_0x17: return read_URM04_dist(USART5, 0x17);
		case i2cU5_0x18: return read_URM04_dist(USART5, 0x18);
		case i2cU5_0x19: return read_URM04_dist(USART5, 0x19);
		case i2cU5_0x1A: return read_URM04_dist(USART5, 0x1A);
		case i2cU5_0x1B: return read_URM04_dist(USART5, 0x1B);
		case i2cU5_0x1C: return read_URM04_dist(USART5, 0x1C);
		case i2cU5_0x1D: return read_URM04_dist(USART5, 0x1D);
		case i2cU5_0x1E: return read_URM04_dist(USART5, 0x1E);
		case i2cU5_0x1F: return read_URM04_dist(USART5, 0x1F);
		case i2cU5_0x20: return read_URM04_dist(USART5, 0x20);
		case i2cU6_0x11: return read_URM04_dist(USART6, 0x11);
		case i2cU6_0x12: return read_URM04_dist(USART6, 0x12);
		case i2cU6_0x13: return read_URM04_dist(USART6, 0x13);
		case i2cU6_0x14: return read_URM04_dist(USART6, 0x14);
		case i2cU6_0x15: return read_URM04_dist(USART6, 0x15);
		case i2cU6_0x16: return read_URM04_dist(USART6, 0x16);
		case i2cU6_0x17: return read_URM04_dist(USART6, 0x17);
		case i2cU6_0x18: return read_URM04_dist(USART6, 0x18);
		case i2cU6_0x19: return read_URM04_dist(USART6, 0x19);
		case i2cU6_0x1A: return read_URM04_dist(USART6, 0x1A);
		case i2cU6_0x1B: return read_URM04_dist(USART6, 0x1B);
		case i2cU6_0x1C: return read_URM04_dist(USART6, 0x1C);
		case i2cU6_0x1D: return read_URM04_dist(USART6, 0x1D);
		case i2cU6_0x1E: return read_URM04_dist(USART6, 0x1E);
		case i2cU6_0x1F: return read_URM04_dist(USART6, 0x1F);
		case i2cU6_0x20: return read_URM04_dist(USART6, 0x20);
		case i2cU7_0x11: return read_URM04_dist(USART7, 0x11);
		case i2cU7_0x12: return read_URM04_dist(USART7, 0x12);
		case i2cU7_0x13: return read_URM04_dist(USART7, 0x13);
		case i2cU7_0x14: return read_URM04_dist(USART7, 0x14);
		case i2cU7_0x15: return read_URM04_dist(USART7, 0x15);
		case i2cU7_0x16: return read_URM04_dist(USART7, 0x16);
		case i2cU7_0x17: return read_URM04_dist(USART7, 0x17);
		case i2cU7_0x18: return read_URM04_dist(USART7, 0x18);
		case i2cU7_0x19: return read_URM04_dist(USART7, 0x19);
		case i2cU7_0x1A: return read_URM04_dist(USART7, 0x1A);
		case i2cU7_0x1B: return read_URM04_dist(USART7, 0x1B);
		case i2cU7_0x1C: return read_URM04_dist(USART7, 0x1C);
		case i2cU7_0x1D: return read_URM04_dist(USART7, 0x1D);
		case i2cU7_0x1E: return read_URM04_dist(USART7, 0x1E);
		case i2cU7_0x1F: return read_URM04_dist(USART7, 0x1F);
		case i2cU7_0x20: return read_URM04_dist(USART7, 0x20);
		default:
			break;
		}
	}
	else
	{
		return DEV_ADDR_ERROR;
	}

	return NO_ERROR;
}

/// Send data to MSP430 via USB
uint32_t send_USBMSP(QByteArray const &i2c_data)
{
	switch (i2c_data.size())
	{
		case 3:
			power_Motor(i2c_data);
			reset_Encoder(i2c_data);
			break;
		case 4:
			freq_Motor(i2c_data);
			break;
		default:
			break;
	}
	return NO_ERROR;
}

/// Read data from MSP430 via USB
uint32_t read_USBMSP(QByteArray const &i2c_data)
{
	switch (i2c_data.size())
	{
		case 2:
			return read_Sensor(i2c_data);
		case 3:
			return read_Encoder(i2c_data);
		default:
			return NO_ERROR;
	}
}
