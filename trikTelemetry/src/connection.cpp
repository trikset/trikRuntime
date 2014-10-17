#include "connection.h"

namespace trikTelemetry {

Connection::Connection(trikControl::Brick &brick)
	: trikKernel::Connection(trikKernel::Protocol::messageLength)
	, mBrick(brick)
{
}

void Connection::processData(QByteArray const &data)
{
	QString command(data);

	QString sensorPortsRequested("sensorPorts:");
	QString sensorRequested("sensor:");
	QString accelerometerRequested("accelerometer");
	QString gyroscopeRequested("gyroscope");

	QString answer;

	if (command.startsWith(sensorPortsRequested)) {
		QString portType = command.remove(sensorPortsRequested.length());
		if (portType == "analog") {
			answer = "sensorPorts:analog:" + mBrick.sensorPorts(trikControl::Sensor::analogSensor).join(',');
		} else if (portType == "digital") {
			answer = "sensorPorts:digital:" + mBrick.sensorPorts(trikControl::Sensor::digitalSensor).join(',');
		} else if (portType == "special") {
			answer = "sensorPorts:special:" + mBrick.sensorPorts(trikControl::Sensor::specialSensor).join(',');
		}
	} else if (command.startsWith(sensorRequested)) {
		QString port = command.remove(0, sensorRequested.length());
		answer = "sensor:" + port + ":" + QString::number(mBrick.sensor(port)->read());
	} else if (command.startsWith(accelerometerRequested)) {
		answer = "accelerometer:" + serializeVector(mBrick.accelerometer()->read());
	} else if (command.startsWith(gyroscopeRequested)) {
		answer = "gyroscope:" + serializeVector(mBrick.gyroscope()->read());
	}

	send(answer.toUtf8());
}

QString Connection::serializeVector(QVector<int> const &vector) {
	QString result = "(";
	for (int coord : vector) {
		result += QString::number(coord) + ",";
	}

	result[result.length() - 1] = ')';
	return result;
}

}
