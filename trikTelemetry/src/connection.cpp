/* Copyright 2014 - 2016 CyberTech Labs Ltd., Grigorii Zimin
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

#include "QsLog.h"

#include <QtCore/QStringBuilder>

using namespace trikTelemetry;

Connection::Connection(trikControl::BrickInterface &brick)
	: trikNetwork::Connection(trikNetwork::Protocol::messageLength, trikNetwork::Heartbeat::use)
	, mBrick(brick)
{
}

void Connection::processData(const QByteArray &data)
{
	QString command(data);

	const QString portsRequested("ports");
	const QString dataRequested("data");
	const QString singleSensorRequested("sensor:");
	const QString buttonRequested("button:");
	const QString accelerometerRequested("AccelerometerPort");
	const QString gyroscopeRequested("GyroscopePort");
	const QString gamepadRequested("Gamepad");

	QString answer;
	if (command.startsWith(dataRequested)) {
		auto reportSensorReading = [this, &answer] (const QString &port) {
			answer += QString("%1:%2:%3;")
					.arg(port)
					.arg(mBrick.sensor(port)->read())
					.arg(mBrick.sensor(port)->readRawData());
		};

		answer = "allData:";

		for (const QString &port : mBrick.sensorPorts(trikControl::SensorInterface::Type::analogSensor)) {
			reportSensorReading(port);
		}

		for (const QString &port : mBrick.sensorPorts(trikControl::SensorInterface::Type::digitalSensor)) {
			reportSensorReading(port);
		}

		for (const QString &port : mBrick.sensorPorts(trikControl::SensorInterface::Type::specialSensor)) {
			reportSensorReading(port);
		}

		for (const QString &port : mBrick.encoderPorts()) {
			answer += QString("%1:%2:%3;")
					.arg(port)
					.arg(mBrick.encoder(port)->read())
					.arg(mBrick.encoder(port)->readRawData());
		}

		const QString semicolon = ";";

		answer += QString("AccelerometerPort:") % serializeVector(mBrick.accelerometer()->read()) % semicolon
				% "GyroscopePort:" % serializeVector(mBrick.gyroscope()->read()) % semicolon

				% "GamepadButton1Port:" + QString::number(mBrick.gamepad()->buttonIsPressed(1)) % semicolon
				% "GamepadButton2Port:" + QString::number(mBrick.gamepad()->buttonIsPressed(2)) % semicolon
				% "GamepadButton3Port:" + QString::number(mBrick.gamepad()->buttonIsPressed(3)) % semicolon
				% "GamepadButton4Port:" + QString::number(mBrick.gamepad()->buttonIsPressed(4)) % semicolon
				% "GamepadButton5Port:" + QString::number(mBrick.gamepad()->buttonIsPressed(5)) % semicolon
				% "GamepadWheelPort:" + QString::number(mBrick.gamepad()->wheel()) % semicolon
				% "GamepadConnectionIndicatorPort:" + QString::number(mBrick.gamepad()->isConnected()) % semicolon
				% "GamepadPad1PressedPort:" + QString::number(mBrick.gamepad()->isPadPressed(1)) % semicolon
				% "GamepadPad2PressedPort:" + QString::number(mBrick.gamepad()->isPadPressed(2)) % semicolon
				% "GamepadPad1PosPort:"
						+ QString("(%1,%2)").arg(mBrick.gamepad()->padX(1)).arg(mBrick.gamepad()->padY(1)) % semicolon
				% "GamepadPad2PosPort:"
						+ QString("(%1,%2)").arg(mBrick.gamepad()->padX(2)).arg(mBrick.gamepad()->padY(2)) % semicolon

				% "Left:" + QString::number(mBrick.keys()->isPressed(105)) % semicolon
				% "Up:" + QString::number(mBrick.keys()->isPressed(103)) % semicolon
				% "Down:" + QString::number(mBrick.keys()->isPressed(108)) % semicolon
				% "Enter:" + QString::number(mBrick.keys()->isPressed(28)) % semicolon
				% "Right:" + QString::number(mBrick.keys()->isPressed(106)) % semicolon
				% "Power:" + QString::number(mBrick.keys()->isPressed(116)) % semicolon
				% "Esc:" + QString::number(mBrick.keys()->isPressed(1));

	} else if (command.startsWith(portsRequested)) {
		answer = "ports:";
		answer += "analog:" + mBrick.sensorPorts(trikControl::SensorInterface::Type::analogSensor).join(",") + ";";
		answer += "digital:" + mBrick.sensorPorts(trikControl::SensorInterface::Type::digitalSensor).join(",") + ";";
		answer += "special:" + mBrick.sensorPorts(trikControl::SensorInterface::Type::specialSensor).join(",") + ";";
		answer += "encoders:" + mBrick.encoderPorts().join(",");
	} else if (command.startsWith(singleSensorRequested)) {
		answer = command + ":";
		command.remove(0, singleSensorRequested.length());
		if (command.startsWith(accelerometerRequested)) {
			const int dimension = command.at(command.length() - 1).toLatin1() - 'X';
			answer += QString::number(mBrick.accelerometer()->read()[dimension]);
		} else if (command.startsWith(gyroscopeRequested)) {
			const int dimension = command.at(command.length() - 1).toLatin1() - 'X';
			answer += QString::number(mBrick.gyroscope()->read()[dimension]);
		} else if (command.startsWith(gamepadRequested)) {
			if (command == "GamepadButton1Port") {
				answer += QString::number(mBrick.gamepad()->buttonIsPressed(1));
			} else if (command == "GamepadButton2Port") {
				answer += QString::number(mBrick.gamepad()->buttonIsPressed(2));
			} else if (command == "GamepadButton3Port") {
				answer += QString::number(mBrick.gamepad()->buttonIsPressed(3));
			} else if (command == "GamepadButton4Port") {
				answer += QString::number(mBrick.gamepad()->buttonIsPressed(4));
			} else if (command == "GamepadButton5Port") {
				answer += QString::number(mBrick.gamepad()->buttonIsPressed(5));
			} else if (command == "GamepadWheelPort") {
				answer += QString::number(mBrick.gamepad()->wheel());
			} else if (command == "GamepadConnectionIndicatorPort") {
				answer += QString::number(mBrick.gamepad()->isConnected());
			} else if (command == "GamepadPad1PressedPort") {
				answer += QString::number(mBrick.gamepad()->isPadPressed(1));
			} else if (command == "GamepadPad2PressedPort") {
				answer += QString::number(mBrick.gamepad()->isPadPressed(2));
			} else if (command == "GamepadPad1PosPort") {
				answer += QString("(%1,%2)").arg(mBrick.gamepad()->padX(1)).arg(mBrick.gamepad()->padY(1));
			} else if (command == "GamepadPad2PosPort") {
				answer += QString("(%1,%2)").arg(mBrick.gamepad()->padX(2)).arg(mBrick.gamepad()->padY(2));
			}
		} else if (mBrick.sensorPorts(trikControl::SensorInterface::Type::analogSensor).contains(command)
				|| mBrick.sensorPorts(trikControl::SensorInterface::Type::digitalSensor).contains(command)
				|| mBrick.sensorPorts(trikControl::SensorInterface::Type::specialSensor).contains(command))
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

QString Connection::serializeVector(const QVector<int> &vector) {
	QString result = "(";
	for (int coord : vector) {
		result += QString::number(coord) + ",";
	}

	result[result.length() - 1] = ')';
	return result;
}

bool Connection::isButtonPressed(const QString &buttonName)
{
	if (buttonName == "Left") {
		return mBrick.keys()->isPressed(105);
	} else if (buttonName == "Up") {
		return mBrick.keys()->isPressed(103);
	} else if (buttonName == "Down") {
		return mBrick.keys()->isPressed(108);
	} else if (buttonName == "Enter") {
		return mBrick.keys()->isPressed(28);
	} else if (buttonName == "Right") {
		return mBrick.keys()->isPressed(106);
	} else if (buttonName == "Power") {
		return mBrick.keys()->isPressed(116);
	} else if (buttonName == "Esc") {
		return mBrick.keys()->isPressed(1);
	} else {
		return false;
	}
}
