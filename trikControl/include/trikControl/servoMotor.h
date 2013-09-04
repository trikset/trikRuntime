#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QFile>

#include "declSpec.h"

namespace trikControl {

/// Generic TRIK servomotor.
class TRIKCONTROL_EXPORT ServoMotor : public QObject
{
	Q_OBJECT

public:
	/// Initializes motor.
	/// @param controlFile Device file for this motor.
	void init(int powerMin, int powerMax, QString const& controlFile);

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	/// @param power Power of a motor, from -100 (full reverse) to 100 (full forward), 0 --- break.
	void setPower(int power);

	/// Returns currently set power of a motor.
	int power() const;

	/// Turns off motor. This is not the same as setPower(0), because setPower will
	/// leave motor on in a break mode, and this method will turn motor off.
	void powerOff();

private:
	QFile mControlFile;
	int mPowerMax;
	int mPowerMin;
	int mPower;
};

}
