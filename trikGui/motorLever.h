/* Copyright 2014 Roman Kurbatov
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

#include <QtCore/qglobal.h>
#include <QObject>
#include <QtCore/QString>

namespace trikControl {
	class MotorInterface;
}

namespace trikGui {

/// Widget that allows to set power value of a motor and turn it on and off.
/// It is designed to use as a part of MotorsWidget.
class MotorLever : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString nameLabel READ nameLabel CONSTANT)
	Q_PROPERTY(bool isOn READ isOn WRITE setIsOn NOTIFY isOnChanged)
	Q_PROPERTY(int maxPower READ maxPower CONSTANT)
	Q_PROPERTY(int minPower READ minPower CONSTANT)
	Q_PROPERTY(int powerStep READ powerStep CONSTANT)
	Q_PROPERTY(int power READ power WRITE setPower NOTIFY powerChanged)

public:
	/// Constructor.
	/// @param port - name of a port which the motor is connected to.
	/// @param motor - pointer to an instance representing the motor.
	/// @param parent - pointer to a parent widget.
	MotorLever(const QString &port, trikControl::MotorInterface &motor, QObject *parent = nullptr);

	/// Destructor.
	~MotorLever() override;

private:
	void setPower(int power);
	void setIsOn(bool isOn);

	QString nameLabel();
	bool isOn();
	int maxPower();
	int minPower();
	int powerStep();
	int power();

	trikControl::MotorInterface &mMotor;
	bool mIsOn;
	const int mMaxPower;
	const int mMinPower;
	const int mPowerStep;
	int mPower;
	QString mNameLabel;
Q_SIGNALS:
	/// Emitted when activation status changed
	void isOnChanged();
	/// Emitted when power changed
	void powerChanged();
};

}
