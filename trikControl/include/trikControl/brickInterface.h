/* Copyright 2015 CyberTech Labs Ltd.
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

#include <trikKernel/lazyMainWidget.h>

#include "batteryInterface.h"
#include "colorSensorInterface.h"
#include "displayInterface.h"
#include "encoderInterface.h"
#include "gamepadInterface.h"
#include "keysInterface.h"
#include "ledInterface.h"
#include "lineSensorInterface.h"
#include "mailboxInterface.h"
#include "motorInterface.h"
#include "objectSensorInterface.h"
#include "pwmCaptureInterface.h"
#include "sensorInterface.h"
#include "vectorSensorInterface.h"

#include "declSpec.h"

namespace trikControl {

/// Interface representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
class TRIKCONTROL_EXPORT BrickInterface : public QObject
{
	Q_OBJECT

public:
	/// Do reset (stop motors, reset keys, clear screen, etc). We should call it before executing any script
	/// with this instance.
	virtual void reset() = 0;

	/// Returns a main GraphicsWidget.
	virtual trikKernel::LazyMainWidget &graphicsWidget() = 0;

public slots:
	/// Plays given music file on a speaker (in format accepted by aplay utility).
	virtual void playSound(QString const &soundFileName) = 0;

	/// Uses text synthesis to say given text on a speaker.
	virtual void say(QString const &text) = 0;

	/// Stops all motors and shuts down all current activity.
	virtual void stop() = 0;

	/// Returns reference to motor of a given type on a given port
	virtual MotorInterface *motor(QString const &port) = 0;

	/// Returns reference to PWM signal capture device on a given port.
	virtual PwmCaptureInterface *pwmCapture(QString const &port) = 0;

	/// Returns reference to sensor on a given port.
	virtual SensorInterface *sensor(QString const &port) = 0;

	/// Retruns list of ports for motors of a given type.
	virtual QStringList motorPorts(MotorInterface::Type type) const = 0;

	/// Returns list of PWM signal capture device ports.
	virtual QStringList pwmCapturePorts() const = 0;

	/// Returns list of ports for sensors of a given type.
	virtual QStringList sensorPorts(SensorInterface::Type type) const = 0;

	/// Returns list of encoder ports
	virtual QStringList encoderPorts() const = 0;

	/// Returns reference to on-board accelerometer.
	virtual VectorSensorInterface *accelerometer() = 0;

	/// Returns reference to on-board gyroscope.
	virtual VectorSensorInterface *gyroscope() = 0;

	/// Returns reference to high-level line detector sensor using camera.
	virtual LineSensorInterface *lineSensor() = 0;

	/// Returns reference to high-level color sensor using camera.
	virtual ColorSensorInterface *colorSensor() = 0;

	/// Returns reference to high-level object detector sensor using USB camera.
	virtual ObjectSensorInterface *objectSensor() = 0;

	/// Returns reference to encoder on given port.
	virtual EncoderInterface *encoder(QString const &port) = 0;

	/// Returns reference to battery.
	virtual BatteryInterface *battery() = 0;

	/// Returns reference to keys on a control brick.
	virtual KeysInterface *keys() = 0;

	/// Returns reference to external gamepad connected via TCP.
	virtual GamepadInterface *gamepad() = 0;

	/// Returns reference to class that provides drawing on display.
	virtual DisplayInterface *display() = 0;

	/// Returns reference to LED control class.
	virtual LedInterface *led() = 0;

	/// Returns reference to mailbox used to send and receive messages to/from other robots.
	virtual MailboxInterface *mailbox() = 0;

signals:
	/// Emitted when all deferred deinitialization is completed and brick completely stopped. Note that if there is no
	/// deferred deinitialization (no video sensors are on, for example), signal will NOT be emitted.
	void stopped();
};

}
