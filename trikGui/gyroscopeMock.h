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
#pragma once
#include "trikControl/gyroSensorInterface.h"
using namespace trikControl;
namespace trikGui {
/// Widget that shows mock gyroscope reading.
class GyroscopeMock : public GyroSensorInterface
{
	Q_OBJECT
public:
	/// Constructor
	GyroscopeMock(QObject *parent = nullptr);
	Status status() const override;
public Q_SLOTS:
	void calibrate(int msec) override;
	QVector<int> getCalibrationValues() override;
	void setCalibrationValues(const QVector<int> &values) override;
	bool isCalibrated() const override;
	QVector<int> readRawData() const override;
	QVector<int> read() const override;
};

} // namespace trikGui
