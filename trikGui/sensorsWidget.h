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
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
#endif

#include <QtCore/QTimer>
#include <QtCore/QVector>

#include "trikGuiDialog.h"

namespace trikControl {
class BrickInterface;
}

namespace trikGui {

class AbstractIndicator;

/// Widget that shows current readings of selected sensors.
class SensorsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	enum class SensorType {
		analogOrDigitalSensor
		, encoder
		, gyroscope
		, accelerometer
		, camera
		, pwmCapture
	};

	/// Constructor.
	explicit SensorsWidget(trikControl::BrickInterface &brick, const QStringList &ports
			, SensorType sensorType, QWidget *parent = 0);

	~SensorsWidget() override;

	int exec() override;

	void renewFocus() override;

protected:
	void exit() override;

	void goHome() override;

	void keyPressEvent(QKeyEvent *event) override;

private:
	AbstractIndicator *produceIndicator(const QString &port, SensorType sensorType);

	trikControl::BrickInterface &mBrick;
	QVBoxLayout mLayout;
	QVector<AbstractIndicator *> mIndicators;  // Has ownership.
	const int mInterval;
	QTimer mTimer;
	SensorType mSensorType;
};

}
