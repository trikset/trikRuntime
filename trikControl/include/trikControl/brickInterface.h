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
#include "markerInterface.h"
#include "objectSensorInterface.h"
#include "pwmCaptureInterface.h"
#include "sensorInterface.h"
#include "soundSensorInterface.h"
#include "vectorSensorInterface.h"
#include "cameraDeviceInterface.h"
#include "lidarInterface.h"
#include "i2cDeviceInterface.h"
#include "irCameraInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Interface representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
class TRIKCONTROL_EXPORT BrickInterface : public QObject
{
	Q_OBJECT

public:
	/// Registers required metatypes
	BrickInterface();

	/// Do reset (stop motors, reset keys, clear screen, etc).
	/// Should be called before executing any script
	/// with this instance.
	/// Must emit resetCompleted signal
	virtual void reset() = 0;

	/// Returns a widget on which display output is drawn.
	virtual DisplayWidgetInterface *graphicsWidget() = 0;

	/// Returns version of system configuration file.
	virtual QString configVersion() const = 0;

public Q_SLOTS:
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
	virtual trikControl::MotorInterface *motor(const QString &port) = 0;

	/// Stops listening given event device and destroys its watcher object.
	virtual void stopEventDevice(const QString &deviceFile) = 0;

public:
	/// Returns reference to PWM signal capture device on a given port.
	Q_INVOKABLE virtual trikControl::PwmCaptureInterface *pwmCapture(const QString &port) = 0;

	/// Returns reference to sensor on a given port.
	Q_INVOKABLE virtual trikControl::SensorInterface *sensor(const QString &port) = 0;

	/// Retruns list of ports for motors of a given type.
	Q_INVOKABLE virtual QStringList motorPorts(MotorInterface::Type type) const = 0;

	/// Returns list of PWM signal capture device ports.
	Q_INVOKABLE virtual QStringList pwmCapturePorts() const = 0;

	/// Returns list of ports for sensors of a given type.
	Q_INVOKABLE virtual QStringList sensorPorts(SensorInterface::Type type) const = 0;

	/// Returns list of encoder ports
	Q_INVOKABLE virtual QStringList encoderPorts() const = 0;

	/// Returns on-board accelerometer.
	Q_INVOKABLE virtual trikControl::VectorSensorInterface *accelerometer() = 0;

	/// Returns on-board gyroscope.
	Q_INVOKABLE virtual trikControl::GyroSensorInterface *gyroscope() = 0;

	/// Returns high-level line detector sensor using camera on given port (video0 or video1).
	Q_INVOKABLE virtual trikControl::LineSensorInterface *lineSensor(const QString &port) = 0;

	/// Returns high-level color sensor using camera on given port (video0 or video1).
	Q_INVOKABLE virtual trikControl::ColorSensorInterface *colorSensor(const QString &port) = 0;

	/// Returns high-level object detector sensor using camera on given port (video0 or video1).
	Q_INVOKABLE virtual trikControl::ObjectSensorInterface *objectSensor(const QString &port) = 0;

	/// Returns lidar on given port.
	Q_INVOKABLE virtual trikControl::LidarInterface *lidar() = 0;

	/// Returns i2c device object
	Q_INVOKABLE virtual trikControl::I2cDeviceInterface *i2c(int bus, int address, int regSize = 8) = 0;

	/// Returns QVector<uin8_t> with image using camera on given port (video0 or video1).
	Q_INVOKABLE virtual QVector<uint8_t> getStillImage() = 0;

	/// Returns high-level sound detector sensor using microphones.
	Q_INVOKABLE virtual trikControl::SoundSensorInterface *soundSensor(const QString &port) = 0;

	/// Returns encoder on given port.
	Q_INVOKABLE virtual trikControl::EncoderInterface *encoder(const QString &port) = 0;

	/// Returns battery.
	Q_INVOKABLE virtual trikControl::BatteryInterface *battery() = 0;

	/// Returns keys on a control brick.
	Q_INVOKABLE virtual trikControl::KeysInterface *keys() = 0;

	/// Returns class that provides drawing on display.
	Q_INVOKABLE virtual trikControl::DisplayInterface *display() = 0;

	/// Returns LED control class.
	Q_INVOKABLE virtual trikControl::LedInterface *led() = 0;

	/// Returns handler for Android gamepad.
	Q_INVOKABLE virtual trikControl::GamepadInterface *gamepad() = 0;

	/// Returns custom FIFO file which can be used as sensor.
	Q_INVOKABLE virtual trikControl::FifoInterface *fifo(const QString &port) = 0;

	/// Returns marker.
	Q_INVOKABLE virtual trikControl::MarkerInterface *marker() = 0;

	/// Returns IR camera
	Q_INVOKABLE virtual trikControl::IrCameraInterface *irCamera() = 0;

	/// Returns custom event device that can be used as a sensor, for example, for custom gamepad support.
	/// Creates new event device on first access to a file, then returns already opened device.
	/// Ownership retained by brick.
	Q_INVOKABLE virtual trikControl::EventDeviceInterface *eventDevice(const QString &deviceFile) = 0;


Q_SIGNALS:
	/// Emitted when all deferred deinitialization is completed and brick completely stopped. Note that if there is no
	/// deferred deinitialization (no video sensors are on, for example), signal will NOT be emitted.
	void stopped();

	/// Emitted when brick finished resetting to default stopped state
	void resetCompleted();
};

}

Q_DECLARE_METATYPE(trikControl::BrickInterface *)
