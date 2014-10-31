#include "connection.h"

using namespace trikTelemetry;

Connection::Connection(trikControl::Brick &brick)
	: trikKernel::Connection(trikKernel::Protocol::messageLength)
	, mBrick(brick)
{
}

void Connection::processData(QByteArray const &data)
{
	QString command(data);

	QString const portsRequested("ports");
	QString const dataRequested("data");
	QString const singleSensorRequested("sensor:");
	QString const accelerometerRequested("AccelerometerPort");
	QString const gyroscopeRequested("GyroscopePort");

	QString answer;

	if (command.startsWith(dataRequested)) {
		auto reportSensorReading = [this, &answer] (QString const &port) {
			answer += QString("%1=%2:%3,")
					.arg(port)
					.arg(mBrick.sensor(port)->read())
					.arg(mBrick.sensor(port)->readRawData());
		};

		answer = "data:";
		answer += "analog:";
		for (QString const &port : mBrick.sensorPorts(trikControl::Sensor::analogSensor)) {
			reportSensorReading(port);
		}

		answer[answer.length() - 1] = ';';
		answer += "digital:";
		for (QString const &port : mBrick.sensorPorts(trikControl::Sensor::digitalSensor)) {
			reportSensorReading(port);
		}

		answer[answer.length() - 1] = ';';
		answer += "special:";
		for (QString const &port : mBrick.sensorPorts(trikControl::Sensor::specialSensor)) {
			reportSensorReading(port);
		}

		answer[answer.length() - 1] = ';';
		answer += "encoders:";
		for (QString const &port : mBrick.encoderPorts()) {
			answer += QString("%1=%2:%3,")
					.arg(port)
					.arg(mBrick.encoder(port)->read())
					.arg(mBrick.encoder(port)->readRawData());
		}

		answer[answer.length() - 1] = ';';
		answer += "accelerometer:" + serializeVector(mBrick.accelerometer()->read()) + ";";
		answer += "gyroscope:" + serializeVector(mBrick.gyroscope()->read());
	} else if (command.startsWith(portsRequested)) {
		answer = "ports:";
		answer += "analog:" + mBrick.sensorPorts(trikControl::Sensor::analogSensor).join(",") + ";";
		answer += "digital:" + mBrick.sensorPorts(trikControl::Sensor::digitalSensor).join(",") + ";";
		answer += "special:" + mBrick.sensorPorts(trikControl::Sensor::specialSensor).join(",") + ";";
		answer += "encoders:" + mBrick.encoderPorts().join(",");
	} else if (command.startsWith(singleSensorRequested)) {
		answer = command + ":";
		command.remove(0, singleSensorRequested.length());
		if (command.startsWith(accelerometerRequested)) {
			int const dimension = command.at(command.length() - 1).toLatin1() - 'X';
			answer += QString::number(mBrick.accelerometer()->read()[dimension]);
		} else if (command.startsWith(gyroscopeRequested)) {
			int const dimension = command.at(command.length() - 1).toLatin1() - 'X';
			answer += QString::number(mBrick.gyroscope()->read()[dimension]);
		} else if (mBrick.sensorPorts(trikControl::Sensor::analogSensor).contains(command)
				|| mBrick.sensorPorts(trikControl::Sensor::digitalSensor).contains(command)
				|| mBrick.sensorPorts(trikControl::Sensor::specialSensor).contains(command))
		{
			answer += QString::number(mBrick.sensor(command)->read());
		} else if (mBrick.encoderPorts().contains(command)) {
			answer += QString::number(mBrick.encoder(command)->read());
		}
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
