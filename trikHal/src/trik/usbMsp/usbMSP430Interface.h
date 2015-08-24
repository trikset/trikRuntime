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
 * Created on: Fabruary 12, 2015
 *	Author: Rostislav Varzar
*/

#pragma once

/// Extract number from packet
uint32_t hex2num(char *string				// Input string
		, uint16_t pos				// Start position
		, uint16_t numsize);			// Number size

/// Make write register packet
void makeWriteRegPacket(char *msp_packet		// Created packet string
			, uint8_t dev_addr		// Device address
			, uint8_t reg_addr		// Registers address to write
			, uint32_t reg_val);		// Value to write

/// Make read register packet
void makeReadRegPacket(char *msp_packet			// Created packet string
			, uint8_t dev_addr		// Device address
			, uint8_t reg_addr);		// Register address to read

/// Init USB TTY device
uint32_t init_USBTTYDevice();

/// Send USB packet
uint32_t sendUSBPacket(char *in_msp_packet		// Packet to send
			, char *out_msp_packet);	// Received packet

/// Function for decoding received packet
uint32_t decodeReceivedPacket(char *msp_packet		// Input MSP430 USB packet string
				, uint8_t &dev_addr	// Decoded response device address
				, uint8_t &func_code	// Decoded function number (read/write)
				, uint8_t &reg_addr	// Decoded register address
				, uint32_t &reg_val);	// Decoded register value

/// Init motors
uint32_t init_motors_USBMSP();

/// Init servo motors
uint32_t init_servomotors_USBMSP();

/// Init encoders
uint32_t init_encoders_USBMSP();

/// Init sensors
uint32_t init_sensors_USBMSP();

/// Init DHTxx sensors
uint32_t init_dhtxx_sensors_USBMSP();

/// Init I2C sensors
uint32_t init_i2c_sensors_USBMSP();

/// Motor power control function
uint32_t power_Motor(QByteArray const &i2c_data);

/// Set motor frequency function
uint32_t freq_Motor(QByteArray const &i2c_data);

/// Reset encoder function
uint32_t reset_Encoder(QByteArray const &i2c_data);

/// Read encoder function
uint32_t read_Encoder(QByteArray const &i2c_data);

/// Read sensor function
uint32_t read_Sensor(QByteArray const &i2c_data);

/// Init I2C + USART + URM04
uint32_t init_URM04(uint8_t i2c_addr, uint8_t usart_addr);

/// Read URM04 distance function
uint32_t read_URM04_dist(uint8_t dev_addr, uint8_t urm04_addr);

/// Connect to USB MSP430 device
uint32_t connect_USBMSP();

/// Disconnect from USB MSP430 device
uint32_t disconnect_USBMSP();

/// Send data to MSP430 via USB
uint32_t send_USBMSP(QByteArray const &i2c_data);

/// Read data from MSP430 via USB
uint32_t read_USBMSP(QByteArray const &i2c_data);

