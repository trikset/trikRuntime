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
#include <QtCore/QString>
#include <trikControl/sensorInterface.h>

using namespace trikGui;

SensorIndicator::SensorIndicator(const QString &port, trikControl::SensorInterface &sensor, QObject *parent)
	: AbstractIndicator(parent), mSensor(sensor), mMinValue(sensor.minValue()), mMaxValue(sensor.maxValue()), mValue(0),
	  mNameLabel(port) {}

void SensorIndicator::renew() {
	mValue = mSensor.read();
	Q_EMIT valueChanged();
}

int SensorIndicator::maxValue() { return mMaxValue; }
int SensorIndicator::minValue() { return mMinValue; }
int SensorIndicator::value() { return mValue; }
QString SensorIndicator::nameLabel() { return mNameLabel; }
