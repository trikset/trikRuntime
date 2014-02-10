/* Copyright 2013 Roman Kurbatov
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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
	#include <QtGui/QProgressBar>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QProgressBar>
#endif

#include <QtCore/QString>

#include <trikControl/powerMotor.h>

namespace trikGui {

class MotorLever : public QWidget
{
	Q_OBJECT

public:
	MotorLever(QString const &name, trikControl::PowerMotor *powerMotor, QWidget *parent = 0);
	~MotorLever();

protected:
	void keyPressEvent(QKeyEvent *event);
	void paintEvent(QPaintEvent *);

private:
	trikControl::PowerMotor *mPowerMotor;
	bool mIsOn;
	int const mMaxPower;
	int const mMinPower;
	int const mPowerState;
	int mPower;

	QVBoxLayout mLayout;
	QLabel mNameLabel;
	QProgressBar mPowerBar;
	QLabel mPowerLabel;
	QLabel mOnOffLabel;
};

}
