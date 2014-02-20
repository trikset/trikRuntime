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

#include "sensorIndicator.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QStylePainter>
	#include <QtGui/QStyleOptionFocusRect>
#else
	#include <QtWidgets/QStylePainter>
	#include <QtWidgets/QStyleOptionFocusRect>
#endif

#include <QtCore/QString>

#include <trikControl/sensor.h>

using namespace trikGui;

SensorIndicator::SensorIndicator(const QString &port
		, trikControl::Sensor &sensor
		, QWidget *parent)
	: QWidget(parent)
	, mSensor(sensor)
	, mMaxValue(100)
	, mMinValue(0)
	, mNameLabel(port)
	, mValueLabel("0")
{
	mValueBar.setOrientation(Qt::Vertical);
	mValueBar.setMaximum(mMaxValue);
	mValueBar.setMinimum(mMinValue);
	mValueBar.setValue(0);
	mValueBar.setTextVisible(false);

	mNameLabel.setAlignment(Qt::AlignCenter);
	mValueLabel.setAlignment(Qt::AlignCenter);

	mLayout.addWidget(&mNameLabel);
	mLayout.addWidget(&mValueBar, 0, Qt::AlignCenter);
	mLayout.addWidget(&mValueLabel);
	setLayout(&mLayout);

	setFocusPolicy(Qt::StrongFocus);
}

void SensorIndicator::paintEvent(QPaintEvent *)
{
	QStylePainter painter(this);

	if (hasFocus()) {
		QStyleOptionFocusRect option;
		option.initFrom(this);
		option.backgroundColor = palette().color(QPalette::Background);

		painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
	}
}

void SensorIndicator::renew()
{
	int const value = mSensor.read();
	mValueLabel.setText(QString::number(value));
	mValueBar.setValue(value);
}
