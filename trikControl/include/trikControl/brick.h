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
#include <QtCore/QStringList>

#include "declSpec.h"

#include "analogSensor.h"
#include "battery.h"
#include "digitalSensor.h"
#include "display.h"
#include "encoder.h"
#include "gamepad.h"
#include "keys.h"
#include "led.h"
#include "sensor.h"
#include "sensor3d.h"
#include "pwmCapture.h"
#include "motor.h"
#include "cameraLineDetectorSensor.h"

namespace trikControl {

class Configurer;
class I2cCommunicator;
class PowerMotor;
class ServoMotor;

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
	/// @param startDirPath - path to the directory from which the application was executed.
	Brick(QThread &guiThread, QString const &configFilePath, QString const &startDirPath);

	~Brick();

	/// Do reset (stop motors, reset keys, clear screen, etc). We should call it before executing any script
	/// with this Brick instance.
	void reset();

	/// Returns true if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool isInEventDrivenMode() const;

public slots:
	/// Plays given music file on a speaker (in format accepted by aplay utility).
	void playSound(QString const &soundFileName);

	/// Uses text synthesis to say given text on a speaker.
	void say(QString const &text);

	/// Stops all motors and shuts down all current activity.
	void stop();

	/// Returns reference to motor of a given type on a given port
	Motor *motor(QString const &port);

	/// Returns reference to PWM signal capture device on a given port.
	PwmCapture *pwmCapture(QString const &port);

	/// Returns reference to sensor on a given port.
	Sensor *sensor(QString const &port);

	/// Retruns list of ports for motors of a given type.
	QStringList motorPorts(Motor::Type type) const;

	/// Returns list of PWM signal capture device ports.
	QStringList pwmCapturePorts() const;

	/// Returns list of ports for sensors of a given type.
	QStringList sensorPorts(Sensor::Type type) const;

	/// Returns list of encoder ports
	QStringList encoderPorts() const;

	/// Returns reference to on-board accelerometer.
	Sensor3d *accelerometer();

	/// Returns reference to on-board gyroscope.
	Sensor3d *gyroscope();

	/// Returns reference to high-level line detector sensor using USB camera.
	CameraLineDetectorSensor *cameraLineDetector();

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
	CameraLineDetectorSensor *mCameraLineDetectorSensor;  // Has ownership.
	Battery *mBattery;  // Has ownership.
	Keys *mKeys;  // Has ownership.
	Gamepad *mGamepad;  // Has ownership.

	QHash<QString, ServoMotor *> mServoMotors;  // Has ownership.
	QHash<QString, PwmCapture *> mPwmCaptures;  // Has ownership.
	QHash<QString, PowerMotor *> mPowerMotors;  // Has ownership.
	QHash<QString, AnalogSensor *> mAnalogSensors;  // Has ownership.
	QHash<QString, Encoder *> mEncoders;  // Has ownership.
	QHash<QString, DigitalSensor *> mDigitalSensors;  // Has ownership.

	Configurer const * const mConfigurer;  // Has ownership.
	I2cCommunicator *mI2cCommunicator;  // Has ownership.
	Display mDisplay;
	Led *mLed;  // Has ownership.

	/// True, if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool mInEventDrivenMode;
};

}
