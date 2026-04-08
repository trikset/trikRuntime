/* Copyright 2019 Dina Funt
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

#include "gyroscopeIndicator.h"
#include <trikControl/gyroSensorInterface.h>

using namespace trikGui;

GyroscopeIndicator::GyroscopeIndicator(trikControl::GyroSensorInterface *gyroscope, QObject *parent)
	: AbstractIndicator(parent), mGyroscope(gyroscope) {}

void GyroscopeIndicator::renew() {
	auto value = mGyroscope != nullptr ? mGyroscope->read() : QVector<int>();
	if (value.size() < 3) {
		return;
	}

	mCoords = value;
	Q_EMIT coordsChanged();
}

QVector<int> GyroscopeIndicator::coords() { return mCoords; }
