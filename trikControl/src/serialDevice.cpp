#include "serialDevice.h"
#include <QsLog.h>
#include <QFile>

using namespace trikControl;

SerialDevice::SerialDevice(const QString &port, const trikKernel::Configurer &configurer)
	: mSerial(configurer.attributeByPort(port, "file"))
	, mState("SerialDevice")
{
	init();
}

void SerialDevice::init() {
	QFile consoleConfig("/etc/default/ttyS1");
	if (consoleConfig.open(QIODevice::ReadOnly)) {
		QTextStream in(&consoleConfig);
		QString line = in.readLine();
		if (line != "LINE_PROTOCOL=serialDevice" &&
			line != "LINE_PROTOCOL=nothing") {
			QLOG_ERROR() << "Lidar: the serial port ttyS1 is not designated for lidar";
			mState.fail();
			return;
		}
	}

	if (!mSerial.open(QIODevice::ReadOnly)) {
		QLOG_ERROR() << "Error open serial devide" << mSerial.portName()
					 << " in read-only mode: " << mSerial.error();
		return;
	}
	mSerial.setDataBits(QSerialPort::Data8);
	mSerial.setParity(QSerialPort::NoParity);
	mSerial.setStopBits(QSerialPort::OneStop);
	mSerial.setFlowControl(QSerialPort::NoFlowControl);
	mState.ready();
	QLOG_INFO() << "Serial device: opened serial port" << mSerial.portName();
}

void SerialDevice::enableConnect() {
	connect(&mSerial, &QSerialPort::readyRead, this, &SerialDevice::newData);
}

void SerialDevice::begin(int baud) {
	mSerial.setBaudRate(baud);
}

SerialDevice::~SerialDevice()
{
	mSerial.close();
}

SerialDevice::Status SerialDevice::status() const
{
	return mState.status();
}

int SerialDevice::available() {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to get available bytes from serial which is not ready, ignoring";
		return 0;
	}
	return mSerial.bytesAvailable();
}

int SerialDevice::writeByte(const char byte) {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to write to serial which is not ready, ignoring";
		return 0;
	}
	return mSerial.write(&byte, 1);
}

int SerialDevice::writeArray(const QByteArray data) {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to write to serial which is not ready, ignoring";
		return 0;
	}
	return mSerial.write(data);
}

QByteArray SerialDevice::read(const int maxLen) {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from serial which is not ready, ignoring";
		return 0;
	}
	return mSerial.read(maxLen);
}

int SerialDevice::read(char *data, int maxLen) {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from serial which is not ready, ignoring";
		return 0;
	}
	auto bytesRead = mSerial.read(data, maxLen);
	if (bytesRead < 0) {
		QLOG_ERROR() << "Serial device: read failed: " << mSerial.error() << " in " << mSerial.portName();
	}
	return bytesRead;
}

bool SerialDevice::atEnd() {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to check if at end of serial buffer, ignoring";
		return true;
	}
	return mSerial.atEnd();
}

int SerialDevice::readByte() {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from serial which is not ready, ignoring";
		return 0;
	}
	char byte;
	mSerial.read(&byte, 1);
	return static_cast<int>(byte);
}

QByteArray SerialDevice::readAll() {
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from serial which is not ready, ignoring";
		return 0;
	}
	return mSerial.readAll();
}
