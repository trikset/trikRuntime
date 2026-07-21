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

#include "abstractIndicator.h"
#include "trikControl/brickInterface.h"
#include <QAbstractListModel>
#include <QQmlApplicationEngine>
#include <QtCore/QTimer>
#include <QtCore/QVector>

namespace trikControl {
class BrickInterface;
}

namespace trikGui {

class AbstractIndicator;

/// Widget that shows current readings of selected sensors.
class Sensors : public QAbstractListModel
{
	Q_OBJECT

public:
	enum class SensorType {
		Sensors,
		Encoder,
		Gyroscope,
		Accelerometer,
		Camera,
		PwmCapture
	};

	/// Constructor.
	explicit Sensors(trikControl::BrickInterface &brick, const QStringList &ports
			, SensorType sensorType, QQmlApplicationEngine *engine, QObject *parent = nullptr);

	~Sensors() override;

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	Q_INVOKABLE void setQmlParent(QObject *parent);

private:
	AbstractIndicator *produceIndicator(const QString &port, SensorType sensorType);
	AbstractIndicator *produceIndicatorMock(const QString &port, SensorType sensorType);

	trikControl::BrickInterface &mBrick;
	QQmlApplicationEngine *mEngine;
	QVector<AbstractIndicator *> mIndicators; // Has ownership.
	const int mInterval;
	QTimer mTimer;
	SensorType mSensorType;
};

}
