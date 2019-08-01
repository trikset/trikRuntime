/* Copyright 2014 - 2015 Roman Kurbatov and CyberTech Labs Ltd.
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

#include <QtWidgets/QStylePainter>
#include <QtWidgets/QStyleOptionFocusRect>

#include <QtCore/QString>

#include <trikControl/sensorInterface.h>

using namespace trikGui;

SensorIndicator::SensorIndicator(const QString &port
		, trikControl::SensorInterface &sensor
		, QWidget *parent)
	: AbstractIndicator(parent)
	, mSensor(sensor)
	, mNameLabel(port)
	, mValueLabel("0")
{
	mValueBar.setOrientation(Qt::Horizontal);
	mValueBar.setMaximum(sensor.maxValue());
	mValueBar.setMinimum(sensor.minValue());
	mValueBar.setValue(0);
	mValueBar.setTextVisible(false);
	mValueBar.setAlignment(Qt::AlignRight);

	mNameLabel.setAlignment(Qt::AlignLeft);
	mValueLabel.setAlignment(Qt::AlignRight);
	// mValueLabel can change its width during work. It will cause mValueBar
	// width change. To prevent it, we set fixed width for mValueLabel.
	// It is equal to maximum width of the widget which it achieves
	// when the label text is set to "100".
	mValueLabel.setFixedWidth(fontMetricsHorizontalAdvance(this, "WWWW"));
	mLayout.addWidget(&mNameLabel);
	mLayout.addWidget(&mValueBar);
	mLayout.addWidget(&mValueLabel);
	setLayout(&mLayout);

	setFocusPolicy(Qt::StrongFocus);
}

void SensorIndicator::renew()
{
	int value = mSensor.read();
	mValueLabel.setText(QString::number(value));
	value = std::max(value, mValueBar.minimum());
	value = std::min(value, mValueBar.maximum());
	mValueBar.setValue(value);
}
