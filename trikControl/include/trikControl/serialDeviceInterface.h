#pragma once

#include <QtCore/QObject>
#include <QVector>
#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

class TRIKCONTROL_EXPORT SerialDeviceInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public :
	virtual int writeByte(const char byte) = 0;
	virtual int writeArray(const QByteArray data) = 0;
	virtual QByteArray read(const int maxLen) = 0;
	virtual int read(char *data, int maxLen) = 0;
	virtual int readByte() = 0;
	virtual QByteArray readAll() = 0;
	virtual void begin(int baud) = 0;
	virtual int available() = 0;
	virtual bool atEnd() = 0;
	virtual void init() = 0;
	virtual void enableConnect() = 0;

Q_SIGNALS:
	void newData();

};

}

Q_DECLARE_METATYPE(trikControl::SerialDeviceInterface *)
