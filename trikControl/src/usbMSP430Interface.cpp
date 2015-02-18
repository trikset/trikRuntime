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

volatile char fstmp[MAX_STRING_LENGTH];             // Buffer for response packets
volatile FILE *usb_in_descr;                        // Input USB device descriptor
volatile int killflag;                              // Flag to terminate read thread
volatile uint16_t mper;                             // Global PWM motor period

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
    uint8_t crc1 = 0;   // Received cheksum
    uint8_t crc2 = 0;   // Calculated checksum
    uint8_t err = 0;    // Error code
    if (msp_packet[0] != ':')                           // Start condition error
    {
        //return START_ERROR;
    }
    if ((strlen(msp_packet) != RECV_PACK_LEN))            // Incorrect packet length
    {
        qDebug() << strlen(msp_packet);
        qDebug() << strlen(RECV_PACK_LEN);
        //return LENGTH_ERROR;
    }
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
uint32_t sendUSBPacket(char *in_msp_packet, char *out_msp_packet)
{
    uint16_t tout = 0;                                  // Timeout counter
    FILE *usb_descr = fopen(USB_DEV_NAME, "w");
    if (usb_descr == NULL)
        return DEVICE_ERROR;
    fprintf(usb_descr, in_msp_packet);
    fflush(usb_descr);
    fclose(usb_descr);
    while ((tout < TIME_OUT))
    {
        tout++;
    }
    strcpy(out_msp_packet, const_cast<char*>(fstmp));
    return NO_ERROR;
}

/*
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
*/

/*
/// Configure USB stty device
void sttyUSBConfig(char *usb_name)
{
    char s1[MAX_STRING_LENGTH];
    QProcess process;
    sprintf(s1, "stty 921600 -F %s -echo -onlcr ", usb_name);
    process.startDetached(s1);
    process.waitForFinished(1000);
}
*/

/// Init motors
uint32_t init_motors_USBMSP()
{
    char s1[MAX_STRING_LENGTH];
    char s2[MAX_STRING_LENGTH];
    uint32_t errcode;
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
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, MOTOR2, MMPER, mper);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, MOTOR3, MMPER, mper);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, MOTOR4, MMPER, mper);
    errcode = sendUSBPacket(s1, s2);
    return NO_ERROR;
}

/// Init encoders
uint32_t init_encoders_USBMSP()
{
    char s1[MAX_STRING_LENGTH];
    char s2[MAX_STRING_LENGTH];
    uint32_t errcode;
    makeWriteRegPacket(s1, ENCODER1, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, ENCODER2, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, ENCODER3, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, ENCODER4, EECTL, ENC_ENABLE + ENC_2WIRES + ENC_PUPEN + ENC_FALL);
    errcode = sendUSBPacket(s1, s2);
    return NO_ERROR;
}

/// Init sensors
uint32_t init_sensors_USBMSP()
{
    char s1[MAX_STRING_LENGTH];
    char s2[MAX_STRING_LENGTH];
    uint32_t errcode;
    makeWriteRegPacket(s1, SENSOR1, SSCTL, SENS_ENABLE + SENS_READ);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR1, SSIDX, ANALOG_INP);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR2, SSCTL, SENS_ENABLE + SENS_READ);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR2, SSIDX, ANALOG_INP);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR3, SSCTL, SENS_ENABLE + SENS_READ);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR3, SSIDX, ANALOG_INP);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR4, SSCTL, SENS_ENABLE + SENS_READ);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR4, SSIDX, ANALOG_INP);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR5, SSCTL, SENS_ENABLE + SENS_READ);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR5, SSIDX, ANALOG_INP);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR6, SSCTL, SENS_ENABLE + SENS_READ);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR6, SSIDX, ANALOG_INP);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR17, SSCTL, SENS_ENABLE + SENS_READ);
    errcode = sendUSBPacket(s1, s2);
    makeWriteRegPacket(s1, SENSOR17, SSIDX, ANALOG_INP);
    errcode = sendUSBPacket(s1, s2);
    return NO_ERROR;
}


/// Connect to USB MSP430 device
uint32_t connect_USBMSP()
{
    uint32_t errcode;

    // Clear temporary buffer
    sprintf(fstmp, "\0");

    // Set terminate flag for read thread
    killflag = 0x01;

    // Open USB device for input
    usb_in_descr = fopen(USB_DEV_NAME, "r");
    if (usb_in_descr == NULL)
        return DEVICE_ERROR;

    // Start separated thread to read from USB device
    ReadUSBThread *readUSBThread = new ReadUSBThread();
    QObject::connect(readUSBThread, SIGNAL(finished()), readUSBThread, SLOT(deleteLater()));
    readUSBThread->start();

    // Send empty packet
    FILE *usb_descr = fopen(USB_DEV_NAME, "w");
    if (usb_descr == NULL)
        return DEVICE_ERROR;
    fprintf(usb_descr, "\n");
    fflush(usb_descr);
    fclose(usb_descr);

    // Init motors
    errcode = init_motors_USBMSP();

    // Init encoders
    errcode = init_encoders_USBMSP();

    // Init sensors
    errcode = init_sensors_USBMSP();

    return NO_ERROR;
}

/// Disconnect from USB MSP430 device
uint32_t disconnect_USBMSP()
{
    // Reset terminate flag for read thread
    killflag = 0x00;

    // Send empty packet to terminate read function in thread
    FILE *usb_descr = fopen(USB_DEV_NAME, "w");
    if (usb_descr == NULL)
        return DEVICE_ERROR;
    fprintf(usb_descr, "\n");
    fflush(usb_descr);
    fclose(usb_descr);

    // Close input USB descriptor
    if (usb_in_descr == NULL)
        return DEVICE_ERROR;
    fclose(usb_in_descr);

    return NO_ERROR;
}


/// Send data to MSP430 via USB
uint32_t send_USBMSP(QByteArray const &i2c_data)
{
    char s1[MAX_STRING_LENGTH];
    char s2[MAX_STRING_LENGTH];
    uint16_t mdut;
    uint16_t mctl;
    uint16_t ptmp;
    uint32_t errcode;
    int8_t mtmp;
    if (usb_in_descr == NULL)
        return DEVICE_ERROR;
    if (i2c_data.size() == 2)
    {
        if ((i2c_data[0] == i2cMOT1) || (i2c_data[0] == i2cMOT2) || (i2c_data[0] == i2cMOT3) || (i2c_data[0] == i2cMOT4))
        {
            mtmp = i2c_data[1];
            mctl = MOT_ENABLE;
            if ((mtmp == INT8_MIN) || (mtmp == INT8_MAX))
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
        }
        switch (i2c_data[0])
        {
            // Motor power
            case i2cMOT1:
                makeWriteRegPacket(s1, MOTOR1, MMDUT, mdut);
                errcode = sendUSBPacket(s1, s2);
                makeWriteRegPacket(s1, MOTOR1, MMCTL, mctl);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cMOT2:
                makeWriteRegPacket(s1, MOTOR2, MMDUT, mdut);
                errcode = sendUSBPacket(s1, s2);
                makeWriteRegPacket(s1, MOTOR2, MMCTL, mctl);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cMOT3:
                makeWriteRegPacket(s1, MOTOR3, MMDUT, mdut);
                errcode = sendUSBPacket(s1, s2);
                makeWriteRegPacket(s1, MOTOR3, MMCTL, mctl);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cMOT4:
                makeWriteRegPacket(s1, MOTOR4, MMDUT, mdut);
                errcode = sendUSBPacket(s1, s2);
                makeWriteRegPacket(s1, MOTOR4, MMCTL, mctl);
                errcode = sendUSBPacket(s1, s2);
                break;
            // Encoder reset
            case i2cENC1:
                makeWriteRegPacket(s1, ENCODER1, EEVAL, i2c_data[1]);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cENC2:
                makeWriteRegPacket(s1, ENCODER2, EEVAL, i2c_data[1]);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cENC3:
                makeWriteRegPacket(s1, ENCODER3, EEVAL, i2c_data[1]);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cENC4:
                makeWriteRegPacket(s1, ENCODER4, EEVAL, i2c_data[1]);
                errcode = sendUSBPacket(s1, s2);
                break;
        default:
                break;
        }
    }
    else if (i2c_data.size() == 3)
    {
        ptmp = ((i2c_data[2] << 8) | i2c_data[1]);
        switch (i2c_data[0])
        {
            // Motor PWM period
            case i2cPWMMOT1:
                if (ptmp > 0)
                {
                    mper = (uint16_t)(24 / 8 * (float)ptmp);
                    if (mper < 1)
                        mper = 1;
                }
                else
                {
                    mper = 1;
                }
                makeWriteRegPacket(s1, MOTOR1, MMPER, mper);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cPWMMOT2:
                if (ptmp > 0)
                {
                    mper = (uint16_t)(24 / 8 * (float)ptmp);
                    if (mper < 1)
                        mper = 1;
                }
                else
                {
                    mper = 1;
                }
                makeWriteRegPacket(s1, MOTOR2, MMPER, mper);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cPWMMOT3:
                if (ptmp > 0)
                {
                    mper = (uint16_t)(24 / 8 * (float)ptmp);
                    if (mper < 1)
                        mper = 1;
                }
                else
                {
                    mper = 1;
                }
                makeWriteRegPacket(s1, MOTOR3, MMPER, mper);
                errcode = sendUSBPacket(s1, s2);
                break;
            case i2cPWMMOT4:
                if (ptmp > 0)
                {
                    mper = (uint16_t)(24 / 8 * (float)ptmp);
                    if (mper < 1)
                        mper = 1;
                }
                else
                {
                    mper = 1;
                }
                makeWriteRegPacket(s1, MOTOR4, MMPER, mper);
                errcode = sendUSBPacket(s1, s2);
                break;
        default:
                break;
        }
    }
    return NO_ERROR;
}

/// Read data from MSP430 via USB
uint32_t read_USBMSP(QByteArray const &i2c_data)
{
    char s1[MAX_STRING_LENGTH];
    char s2[MAX_STRING_LENGTH];
    uint32_t errcode;
    uint8_t devaddr;
    uint8_t funccode;
    uint8_t regaddr;
    uint32_t regval;
    if (i2c_data.size() == 2)
    {
        switch (i2c_data[0])
        {
            // Encoder read
            case i2cENC1:
                do
                {
                    makeReadRegPacket(s1, ENCODER1, EEVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != ENCODER1) || (regaddr != EEVAL));
                return regval;
            case i2cENC2:
                do
                {
                    makeReadRegPacket(s1, ENCODER2, EEVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != ENCODER2) || (regaddr != EEVAL));
                return regval;
            case i2cENC3:
                do
                {
                    makeReadRegPacket(s1, ENCODER3, EEVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != ENCODER3) || (regaddr != EEVAL));
                return regval;
            case i2cENC4:
                do
                {
                    makeReadRegPacket(s1, ENCODER4, EEVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != ENCODER4) || (regaddr != EEVAL));
                return regval;
            default:
                break;
        }
    }
    else if (i2c_data.size() == 1)
    {
        switch (i2c_data[0])
        {
            // Sensor read
            case i2cSENS1:
                do
                {
                    makeReadRegPacket(s1, SENSOR1, SSVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != SENSOR1) || (regaddr != SSVAL));
                return regval;
            case i2cSENS2:
                do
                {
                    makeReadRegPacket(s1, SENSOR2, SSVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != SENSOR2) || (regaddr != SSVAL));
                return regval;
            case i2cSENS3:
                do
                {
                    makeReadRegPacket(s1, SENSOR3, SSVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != SENSOR3) || (regaddr != SSVAL));
                return regval;
            case i2cSENS4:
                do
                {
                    makeReadRegPacket(s1, SENSOR4, SSVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != SENSOR4) || (regaddr != SSVAL));
                return regval;
            case i2cSENS5:
                do
                {
                    makeReadRegPacket(s1, SENSOR5, SSVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != SENSOR5) || (regaddr != SSVAL));
                return regval;
            case i2cSENS6:
                do
                {
                    makeReadRegPacket(s1, SENSOR6, SSVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != SENSOR6) || (regaddr != SSVAL));
                return regval;
            // Battery read
            case i2cBATT:
                do
                {
                    makeReadRegPacket(s1, SENSOR17, SSVAL);
                    errcode = sendUSBPacket(s1, s2);
                    errcode = decodeReceivedPacket(s2, devaddr, funccode, regaddr, regval);
                } while ((devaddr != SENSOR17) || (regaddr != SSVAL));
                return regval;
            default:
                break;
        }
    }
    return NO_ERROR;
}
