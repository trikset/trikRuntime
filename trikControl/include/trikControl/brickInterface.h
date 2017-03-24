/* Copyright 2015 - 2016 CyberTech Labs Ltd.
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

#include <QtCore/QStringList>

#include "batteryInterface.h"
#include "colorSensorInterface.h"
#include "displayInterface.h"
#include "encoderInterface.h"
#include "eventDeviceInterface.h"
#include "fifoInterface.h"
#include "gamepadInterface.h"
#include "gyroSensorInterface.h"
#include "keysInterface.h"
#include "ledInterface.h"
#include "lineSensorInterface.h"
#include "motorInterface.h"
#include "objectSensorInterface.h"
#include "pwmCaptureInterface.h"
#include "sensorInterface.h"
#include "soundSensorInterface.h"
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

	/// Returns a widget on which display output is drawn.
	virtual DisplayWidgetInterface *graphicsWidget() = 0;

	/// Returns version of system configuration file.
	virtual QString configVersion() const = 0;

public slots:
	/// Configures given device on given port. Port must be listed in model-config.xml, device shall be listed
	/// in system-config.xml, and device shall be able to be configured on a port (it is also described
	/// in system-config.xml). Previously configured device is properly shut down, and new device is created
	/// and initialized on a port. Method blocks caller thread until device is created. Note that this method does not
	/// initialize devices like camera sensors, "init" shall be called for them separately when they are configured
	/// (it is consistent with Brick constructor behavior).
	virtual void configure(const QString &portName, const QString &deviceName) = 0;

	/// Plays given music file on a speaker (in format accepted by aplay or cvlc utilities).
	virtual void playSound(const QString &soundFileName) = 0;

	/// Generates sound with given frequency and given duration, plays it on a speaker.
	virtual void playTone(int hzFreq, int msDuration) = 0;

	/// Uses text synthesis to say given text on a speaker.
	virtual void say(const QString &text) = 0;

	/// Stops all motors and shuts down all current activity.
	virtual void stop() = 0;

	/// Returns reference to motor of a given type on a given port
	virtual MotorInterface *motor(const QString &port) = 0;

	/// Returns reference to PWM signal capture device on a given port.
	virtual PwmCaptureInterface *pwmCapture(const QString &port) = 0;

	/// Returns reference to sensor on a given port.
	virtual SensorInterface *sensor(const QString &port) = 0;

	/// Retruns list of ports for motors of a given type.
	virtual QStringList motorPorts(MotorInterface::Type type) const = 0;

	/// Returns list of PWM signal capture device ports.
	virtual QStringList pwmCapturePorts() const = 0;

	/// Returns list of ports for sensors of a given type.
	virtual QStringList sensorPorts(SensorInterface::Type type) const = 0;

	/// Returns list of encoder ports
	virtual QStringList encoderPorts() const = 0;

	/// Returns on-board accelerometer.
	virtual VectorSensorInterface *accelerometer() = 0;

	/// Returns on-board gyroscope.
	virtual GyroSensorInterface *gyroscope() = 0;

	/// Returns high-level line detector sensor using camera on given port (video0 or video1).
	virtual LineSensorInterface *lineSensor(const QString &port) = 0;

	/// Returns high-level color sensor using camera on given port (video0 or video1).
	virtual ColorSensorInterface *colorSensor(const QString &port) = 0;

	/// Returns high-level object detector sensor using camera on given port (video0 or video1).
	virtual ObjectSensorInterface *objectSensor(const QString &port) = 0;

	/// Returns high-level sound detector sensor using microphones.
	virtual SoundSensorInterface *soundSensor(const QString &port) = 0;

	/// Returns encoder on given port.
	virtual EncoderInterface *encoder(const QString &port) = 0;

	/// Returns battery.
	virtual BatteryInterface *battery() = 0;

	/// Returns keys on a control brick.
	virtual KeysInterface *keys() = 0;

	/// Returns class that provides drawing on display.
	virtual DisplayInterface *display() = 0;

	/// Returns LED control class.
	virtual LedInterface *led() = 0;

	/// Returns handler for Android gamepad.
	virtual GamepadInterface *gamepad() = 0;

	/// Returns custom FIFO file which can be used as sensor.
	virtual FifoInterface *fifo(const QString &port) = 0;

	/// Returns custom event device that can be used as a sensor, for example, for custom gamepad support.
	/// Creates new event device on first access to a file, then returns already opened device.
	/// Ownership retained by brick.
	virtual EventDeviceInterface *eventDevice(const QString &deviceFile) = 0;

	/// Stops listening given event device and destroys its watcher object.
	virtual void stopEventDevice(const QString &deviceFile) = 0;

signals:
	/// Emitted when all deferred deinitialization is completed and brick completely stopped. Note that if there is no
	/// deferred deinitialization (no video sensors are on, for example), signal will NOT be emitted.
	void stopped();
};

}
