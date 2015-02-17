/*
 * usbMSP430Interface.h
 *
 *  Created on: Fabruary 16, 2015
 *      Author: Rostislav Varzar
 */


#ifndef USBMSP430READ_H
#define USBMSP430READ_H

#include <QThread>
#include <QObject>

#define MAX_STRING_LENGTH   0x20

extern volatile char fstmp[MAX_STRING_LENGTH];          // Buffer for response packets
extern volatile FILE *usb_in_descr;                     // Input USB device descriptor
extern volatile int killflag;                           // Flag to terminate read thread

/// Class for reading data from USB in separated thread
class ReadUSBThread : public QThread
{
    Q_OBJECT
    void run() {
        QString result;
        /* expensive or blocking operation  */
        while (killflag)
        {
            fscanf(usb_in_descr, "%s", fstmp);
        }
        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);
};


#endif // USBMSP430READ_H
