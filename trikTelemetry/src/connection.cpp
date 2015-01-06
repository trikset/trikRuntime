/* Copyright 2014 CyberTech Labs Ltd.
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
 * limitations under the License. */

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
	QString const buttonRequested("button:");
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
		} else if (command.startsWith(buttonRequested)) {
			command.remove(0, buttonRequested.length());
			answer = "sensor:" + command + ":" + (isButtonPressed(command) ? "1" : "0");
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

bool Connection::isButtonPressed(const QString &buttonName)
{
	if (buttonName == "LeftButtonPort") {
		return mBrick.keys()->isPressed(105);
	} else if (buttonName == "UpButtonPort") {
		return mBrick.keys()->isPressed(103);
	} else if (buttonName == "DownButtonPort") {
		return mBrick.keys()->isPressed(108);
	} else if (buttonName == "EnterButtonPort") {
		return mBrick.keys()->isPressed(28);
	} else if (buttonName == "RightButtonPort") {
		return mBrick.keys()->isPressed(106);
	} else if (buttonName == "PowerButtonPort") {
		return mBrick.keys()->isPressed(116);
	} else if (buttonName == "MenuButtonPort") {
		return mBrick.keys()->isPressed(139);
	} else {
		return false;
	}
}
