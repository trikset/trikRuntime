/* Copyright 2024 Daniel Chehade.
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

#include "gyroscopeMock.h"
using namespace trikControl;
using namespace trikGui;
GyroscopeMock::GyroscopeMock(QObject *parent) { setParent(parent); }
QVector<int> GyroscopeMock::read() const {
	int x = rand() % 200000 - 100000;
	int y = rand() % 200000 - 100000;
	int z = rand() % 200000 - 100000;
	return QVector<int>{x, y, z};
}

void GyroscopeMock::calibrate(int msec) { Q_UNUSED(msec) }
QVector<int> GyroscopeMock::getCalibrationValues() { return QVector<int>(); }
void GyroscopeMock::setCalibrationValues(const QVector<int> &values) { Q_UNUSED(values) }
bool GyroscopeMock::isCalibrated() const { return true; }
QVector<int> GyroscopeMock::readRawData() const { return QVector<int>(); }

trikControl::DeviceInterface::Status GyroscopeMock::status() const {
	return trikControl::DeviceInterface::Status::ready;
}
