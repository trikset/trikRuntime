#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>

#include "declSpec.h"
#include "servoMotor.h"
#include "powerMotor.h"
#include "sensor.h"

namespace trikControl {

/// Class representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
class TRIKCONTROL_EXPORT Brick : public QObject
{
	Q_OBJECT

public:
	Brick();

public slots:
	/// Plays given music file on a speaker (in format accepted by aplay utility).
	void playSound(QString const &soundFileName);

	/// Stops all motors and shuts down all current activity.
	void stop();

	/// Returns reference to motor on a given port.
	ServoMotor *motor(int const &port);

	/// Returns reference to power motor on a given port.
	PowerMotor *powerMotor(int const &port);

	/// Returns reference to sensor on a given port.
	Sensor *sensor(int const &port);

	/// Waits given amount of time in milliseconds and returns.
	void wait(int const &milliseconds) const;

private:
	class SleeperThread : public QThread
	{
	public:
		static void msleep(unsigned long msecs)
		{
			QThread::msleep(msecs);
		}
	};

	ServoMotor mMotor1;
	ServoMotor mMotor2;
	PowerMotor mPowerMotor1;
	PowerMotor mPowerMotor2;
	PowerMotor mPowerMotor3;
	PowerMotor mPowerMotor4;
	Sensor mSensor1;
	Sensor mSensor2;
};

}
