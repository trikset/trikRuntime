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
	#include <QtGui/QHBoxLayout>
	#include <QtGui/QLabel>
	#include <QtGui/QProgressBar>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QProgressBar>
#endif

namespace trikControl {
class SensorInterface;
}

namespace trikGui {

/// Widget that shows current sensor reading.
class SensorIndicator : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port to which sensor is plugged.
	/// @param sensor - sensor which we will read.
	/// @param parent - parent of this widget in Qt widget parent-child system.
	SensorIndicator(const QString &port, trikControl::SensorInterface &sensor, QWidget *parent = 0);

public slots:
	/// Rereads sensor and updates widget contents.
	void renew();

private:
	trikControl::SensorInterface &mSensor;
	const int mMaxValue;
	const int mMinValue;

	QHBoxLayout mLayout;
	QLabel mNameLabel;
	QProgressBar mValueBar;
	QLabel mValueLabel;
};

}
