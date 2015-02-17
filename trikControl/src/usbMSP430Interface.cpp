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
#include <QThread>
#include <QString>
#include <QObject>
#include <QProcess>
#include "usbMSP430Defines.h"
#include "usbMSP430Interface.h"
#include "usbMSP430Read.h"

// volatile char fstmp[MAX_STRING_LENGTH];          // Buffer for response packets
volatile char fstmp[MAX_STRING_LENGTH];             // Buffer for response packets
volatile FILE *usb_in_descr;                        // Input USB device descriptor
volatile int killflag;                              // Flag to terminate read thread

/// Extract number from packet
uint32_t hex2num(char *string, uint16_t pos, uint16_t numsize)
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
void makeWriteRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr, uint32_t reg_val)
{
    uint8_t crc = (0xFF - (dev_addr + WRITE_FUNC + reg_addr + uint8_t(reg_val & 0xFF) + uint8_t((reg_val >> 8) & 0xFF) +
                   uint8_t((reg_val >> 16) & 0xFF) + uint8_t((reg_val >> 24) & 0xFF)) + 1) & 0xFF;  // Checksum
    sprintf(msp_packet, ":%02X%02X%02X%08X%02X\n", dev_addr, WRITE_FUNC, reg_addr, reg_val, crc);
}

/// Make read register packet
void makeReadRegPacket(char *msp_packet, uint8_t dev_addr, uint8_t reg_addr)
{
    uint8_t crc = (0xFF - (dev_addr + READ_FUNC + reg_addr) + 1) & 0xFF;  // Checksum
    sprintf(msp_packet, ":%02X%02X%02X%02X\n", dev_addr, READ_FUNC, reg_addr, crc);
}

/// Function for decoding received packet
uint32_t decodeReceivedPacket(char *msp_packet, uint8_t &dev_addr, uint8_t &func_code, uint8_t &reg_addr, uint32_t &reg_val)
{
    uint8_t crc1 = 0; //Received cheksum
    uint8_t crc2 = 0; //Calculated checksum
    if (msp_packet[0] != ':')                           // Start condition error
        return START_ERROR;
    if (strlen(msp_packet) != RECV_PACK_LEN)            // Incorrect packet length
        return LENGTH_ERROR;
    dev_addr = hex2num(msp_packet, 1, NUM_BYTE);        // Get device address
    func_code = hex2num(msp_packet, 3, NUM_BYTE);       // Get function
    reg_addr = hex2num(msp_packet, 5, NUM_BYTE);        // Get register address
    reg_val = hex2num(msp_packet, 7, NUM_DWORD);        // Get register value
    crc1 = hex2num(msp_packet, 15, NUM_BYTE);           // Get CRC from packet
    crc2 = (0xFF - (dev_addr + func_code + reg_addr + uint8_t(reg_val & 0xFF) + uint8_t((reg_val >> 8) & 0xFF) +
                       uint8_t((reg_val >> 16) & 0xFF) + uint8_t((reg_val >> 24) & 0xFF)) + 1) & 0xFF;   // Calculate CRC
    if (crc1 != crc2)                                   // Check CRC
        return CRC_ERROR;
    return NO_ERROR;                                    // Return NO ERROR
}

/// Send USB packet
uint32_t sendUSBPacket(char *usb_name, char *in_msp_packet, char *out_msp_packet)
{
    uint16_t tout = 0;                                  // Timeout counter
    FILE *usb_descr = fopen(usb_name, "w");
    fprintf(usb_descr, in_msp_packet);
    fflush(usb_descr);
    fclose(usb_descr);
    while ((tout < TIME_OUT))
    {
        tout++;
    }
    strcpy(out_msp_packet, const_cast<char*>(fstmp));

    return NO_RESP_ERROR;
}

/// Set power 12 volts on
void setPower12V(uint8_t pwr)
{
    char s1[MAX_STRING_LENGTH];
    QProcess process;
    if (pwr)
        sprintf(s1, "echo 1 > /sys/class/gpio/gpio62/value");
    else
        sprintf(s1, "echo 0 > /sys/class/gpio/gpio62/value");
    process.startDetached(s1);
    process.waitForFinished(1000);
}

/// Configure USB stty device
void sttyUSBConfig(char *usb_name)
{
    char s1[MAX_STRING_LENGTH];
    QProcess process;
    sprintf(s1, "stty 921600 -F %s -echo -onlcr ", usb_name);
    process.startDetached(s1);
    process.waitForFinished(1000);
}

/// Connect to USB MSP430 device
void connect_USBMSP(FILE *&usb_out_descr, char *usb_name)
{
    // Clear temporary buffer
    sprintf(fstmp, "");

    // Set terminate flag for read thread
    killflag = 0x01;

    // Open USB device for input
    usb_in_descr = fopen(usb_name, "r");

    // Start separated thread to read from USB device
    ReadUSBThread *readUSBThread = new ReadUSBThread();
    QObject::connect(readUSBThread, SIGNAL(finished()), readUSBThread, SLOT(deleteLater()));
    readUSBThread->start();
}

/// Disconnect from USB MSP430 device
void disconnect_USBMSP(FILE *&usb_out_descr, char *usb_name)
{
    // Reset terminate flag for read thread
    killflag = 0x00;

    // Send empty packet to terminate read function in thread
    usb_out_descr = fopen(usb_name, "w");
    fprintf(usb_out_descr, "\n");
    fclose(usb_out_descr);

    fclose(usb_in_descr);
}


/// Send data to MSP430 via USB
void send_USBMSP(QByteArray const &i2c_data)
{
    char s1[MAX_STRING_LENGTH];
    if (i2c_data.size() == 2)
    {
        switch (i2c_data[0])
        {
            case i2cMOT1:
                makeWriteRegPacket(s1, MOTOR1, MMPER, DEF_MOT_PER);
                makeWriteRegPacket(s1, MOTOR1, MMDUT, DEF_MOT_PER / 2);
                makeWriteRegPacket(s1, MOTOR1, MMCTL, 0x8007);
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
uint32_t read_USBMSP(QByteArray const &i2c_data)
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
