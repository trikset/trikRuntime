#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QFile>

#include "declSpec.h"

namespace trikControl {

/// TRIK power motor.
class TRIKCONTROL_EXPORT PowerMotor : public QObject
{
	Q_OBJECT

public:
	/// Initializes motor.
	void init(int engine);

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
	int mEngine;
	int mPower;
	QString mName;
};

}
