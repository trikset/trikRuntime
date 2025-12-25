#pragma once

#include "serialDeviceInterface.h"
#include <QtCore/QObject>
#include <QSerialPort>
#include "deviceState.h"
#include <trikKernel/configurer.h>

namespace trikControl {

class SerialDevice : public SerialDeviceInterface
{
	Q_OBJECT
public:
	SerialDevice(const QString &port, const trikKernel::Configurer &configurer);
	~SerialDevice();
	Status status() const override;

public Q_SLOTS:
	int writeByte(const char byte);
	int writeArray(const QByteArray data);
	QByteArray read(const int maxLen);
	int read(char *data, int maxLen);
	int readByte();
	QByteArray readAll();
	void begin(int baud);
	int available();
	bool atEnd();
	void init();
	void enableConnect();

Q_SIGNALS:
	void newData();

public:
	QSerialPort mSerial;
	QString mResult;
	DeviceState mState;
	QByteArray mBuffer;

};

}
