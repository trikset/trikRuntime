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

#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QRectF>

#include "abstractIndicator.h"

namespace trikControl {
class VectorSensorInterface;
}

namespace trikGui {

/// Widget that shows current accelerometer reading.
class AccelerometerWidget : public AbstractIndicator
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param accelerometer - accelerometer which we will read.
	/// @param parent - parent of this widget in Qt widget parent-child system.
	explicit AccelerometerWidget(trikControl::VectorSensorInterface *accelerometer, QWidget *parent = 0);

public slots:
	void renew() override;

private:
	void resizeEvent(QResizeEvent *) override;
	void paintEvent(QPaintEvent *) override;

	QRectF mBounds;
	QRectF mKnopBounds;

	QLabel mTitle;
	QLabel mValueX;
	QLabel mValueY;
	QLabel mValueZ;
	QLabel mCircle;
	QVBoxLayout mLayout;

	trikControl::VectorSensorInterface *mAccelerometer;
};
}
