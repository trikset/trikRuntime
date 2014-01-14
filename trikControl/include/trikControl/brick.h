/* Copyright 2013 Yurii Litvinov
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QHash>

#include "declSpec.h"

#include "analogSensor.h"
#include "battery.h"
#include "display.h"
#include "encoder.h"
#include "gamepad.h"
#include "keys.h"
#include "led.h"
#include "powerMotor.h"
#include "sensor.h"
#include "sensor3d.h"
#include "servoMotor.h"
#include "pwmCapture.h"

namespace trikControl {

class Configurer;
class I2cCommunicator;

/// Class representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
class TRIKCONTROL_EXPORT Brick : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param guiThread - thread in which an application has started. Can be obtaned in main() by code like
	///        QApplication app; app.thread();
	/// @param configFilePath - path to config.xml
	Brick(QThread &guiThread, QString const &configFilePath);

	~Brick();

	/// Returns true if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool isInEventDrivenMode() const;

	/// Clears event driven mode, returning brick to a state where a script will exit immediately.
	void resetEventDrivenMode();

public slots:
	/// Plays given music file on a speaker (in format accepted by aplay utility).
	void playSound(QString const &soundFileName);

	/// Stops all motors and shuts down all current activity.
	void stop();

	/// Returns reference to motor on a given port.
	ServoMotor *servoMotor(QString const &port);

	PwmCapture *pwmCapture(QString const &port);

	/// Returns reference to power motor on a given port.
	PowerMotor *powerMotor(QString const &port);

	/// Returns reference to analog sensor on a given port.
	AnalogSensor *analogSensor(QString const &port);

	/// Returns reference to sensor on a given port.
	Sensor *sensor(QString const &port);

	/// Returns reference to on-board accelerometer.
	Sensor3d *accelerometer();

	/// Returns reference to on-board gyroscope.
	Sensor3d *gyroscope();

	/// Returns reference to encoder on given port.
	Encoder *encoder(QString const &port);

	/// Returns reference to battery.
	Battery *battery();

	/// Returns reference to keys on a control brick.
	Keys *keys();

	/// Returns reference to external gamepad connected via TCP.
	Gamepad *gamepad();

	/// Waits given amount of time in milliseconds and returns.
	void wait(int const &milliseconds) const;

	/// Returns the number of milliseconds since 1970-01-01T00:00:00 UTC.
	qint64 time() const;

	/// Returns reference to class that provides drawing on display.
	Display *display();

	/// Returns reference to LED control class.
	Led *led();

	/// Starts event loop for script.
	void run();

	/// Aborts script execution.
	void quit();

	/// Asynchronously execute given sh command.
	void system(QString const &command);

signals:
	/// Emitted when script requested system to abort execution.
	void quitSignal();

private:
	class SleeperThread : public QThread
	{
	public:
		static void msleep(unsigned long msecs)
		{
			QThread::msleep(msecs);
		}
	};

	Sensor3d *mAccelerometer;  // has ownership.
	Sensor3d *mGyroscope;  // has ownership.
	Battery *mBattery;  // Has ownership.
	Keys *mKeys;  // Has ownership.
	Gamepad *mGamepad;  // Has ownership.

	QHash<QString, ServoMotor *> mServoMotors;  // Has ownership.
	QHash<QString, PwmCapture *> mPwmCaptures;
	QHash<QString, PowerMotor *> mPowerMotors;  // Has ownership.
	QHash<QString, AnalogSensor *> mAnalogSensors;  // Has ownership.
	QHash<QString, Encoder *> mEncoders;  // Has ownership.
	QHash<QString, Sensor *> mSensors;  // Has ownership.

	Configurer const * const mConfigurer;  // Has ownership.
	I2cCommunicator *mI2cCommunicator;  // Has ownership.
	Display mDisplay;
	Led *mLed;  // Has ownership.

	/// True, if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool mInEventDrivenMode;
};

}
