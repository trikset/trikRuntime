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

namespace trikControl {
	class Sensor;
}

namespace trikGui {

class SensorIndicator : public QWidget
{
	Q_OBJECT

public:
	SensorIndicator(QString const &port, trikControl::Sensor &sensor, QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *);

private:
	trikControl::Sensor &mSensor;
	int const mMaxValue;
	int const mMinValue;
	int const mInterval;

	QVBoxLayout mLayout;
	QLabel mNameLabel;
	QProgressBar mValueBar;
	QLabel mValueLabel;

public slots:
	void renew();
};

}
