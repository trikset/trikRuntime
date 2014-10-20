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

	QString portsRequested("ports");
	QString dataRequested("data");

	QString answer;

	if (command.startsWith(dataRequested)) {
		answer = "data:";
		answer += "analog:";
		for (QString port : mBrick.sensorPorts(trikControl::Sensor::analogSensor)) {
			answer += port + "=" + QString::number(mBrick.sensor(port)->read()) + ":";
			answer += QString::number(mBrick.sensor(port)->readRawData()) + ",";
		}
		answer[answer.length() - 1] = ';';
		answer += "digital:";
		for (QString port : mBrick.sensorPorts(trikControl::Sensor::digitalSensor)) {
			answer += port + "=" + QString::number(mBrick.sensor(port)->read()) + ":";
			answer += QString::number(mBrick.sensor(port)->readRawData()) + ",";
		}
		answer[answer.length() - 1] = ';';
		answer += "special:";
		for (QString port : mBrick.sensorPorts(trikControl::Sensor::specialSensor)) {
			answer += port + "=" + QString::number(mBrick.sensor(port)->read()) + ":";
			answer += QString::number(mBrick.sensor(port)->readRawData()) + ",";
		}
		answer[answer.length() - 1] = ';';
		answer += "accelerometer:" + serializeVector(mBrick.accelerometer()->read()) + ";";
		answer += "gyroscope:" + serializeVector(mBrick.gyroscope()->read());
	} else if (command.startsWith(portsRequested)) {
		answer = "ports:";
		answer += "analog:" + mBrick.sensorPorts(trikControl::Sensor::analogSensor).join(",") + ";";
		answer += "digital:" + mBrick.sensorPorts(trikControl::Sensor::digitalSensor).join(",") + ";";
		answer += "special:" + mBrick.sensorPorts(trikControl::Sensor::specialSensor).join(",");
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
