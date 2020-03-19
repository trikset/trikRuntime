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

#include <QString>
#include <QPainter>

#include <trikControl/gyroSensorInterface.h>

using namespace trikGui;

static constexpr int MAX_GYROSCOPE_VAL = 100000;
static constexpr int ARC_DRAWING_CONST = 16;

GyroscopeIndicator::GyroscopeIndicator(trikControl::GyroSensorInterface *gyroscope
		, QWidget *parent)
	: AbstractIndicator(parent)
	, mTitle(tr("Gyroscope"))
	, mGyroscope(gyroscope)
{
	mTitle.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	mValueX.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	mValueY.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	mValueZ.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	mCircle.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mValueX);
	mLayout.addWidget(&mValueY);
	mLayout.addWidget(&mValueZ);

	mLayout.addWidget(&mCircle);

	setLayout(&mLayout);
}

void GyroscopeIndicator::renew()
{
	const auto &value = mGyroscope != nullptr ? mGyroscope->read() : QVector<int>();

	const auto &text = [&value](int i, int &out) {
		return value.size() > i ? QString::number(out = value[i]) : tr("Error");
	};

	mValueX.setText(QString("x: ") + text(0, mX));
	mValueY.setText(QString("y: ") + text(1, mY));
	mValueZ.setText(QString("z: ") + text(2, mZ));

	update();
}

void GyroscopeIndicator::resizeEvent(QResizeEvent *)
{
	auto size = qMin(mCircle.width(), mCircle.height());
	QPointF topleft((mCircle.width() - size) / 2, (mCircle.height() - size) / 2);
	mBounds = QRectF(topleft, QSize(size, size));
}

template<typename T>
T constrain(T value, T max, T min)
{
	return (value > max) ? max : (value < min) ? min : value;
}

void GyroscopeIndicator::paintEvent(QPaintEvent *)
{
	QPixmap pixmap(mCircle.size());
	pixmap.fill(Qt::transparent);

	QPainter painter(&pixmap);

	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	painter.setPen(QPen(QBrush(Qt::black), mBounds.width() * 0.003));
	painter.setBrush(QBrush(Qt::transparent));
	painter.drawEllipse(mBounds);

	// draw crosshair
	painter.setPen(QPen(QBrush(Qt::black), mBounds.width() * 0.001));
	painter.drawLine(QPointF(mBounds.left(), mBounds.center().y()), QPointF(mBounds.right(), mBounds.center().y()));
	painter.drawLine(QPointF(mBounds.center().x(), mBounds.top()), QPointF(mBounds.center().x(), mBounds.bottom()));

	// draw x, y lines
	painter.setPen(QPen(QBrush(Qt::blue), mBounds.width() * 0.02));
	auto xShift = constrain(mX, MAX_GYROSCOPE_VAL, -MAX_GYROSCOPE_VAL) * mBounds.width() / (2 * MAX_GYROSCOPE_VAL);
	painter.drawLine(mBounds.center(), QPointF(mBounds.center().x() + xShift, mBounds.center().y()));

	painter.setPen(QPen(QBrush(Qt::green), mBounds.width() * 0.02));
	auto yShift = constrain(mY, MAX_GYROSCOPE_VAL, -MAX_GYROSCOPE_VAL) * mBounds.width() / (2 * MAX_GYROSCOPE_VAL);
	painter.drawLine(mBounds.center(), QPointF(mBounds.center().x(), mBounds.center().y() + yShift));

	// draw z arc
	painter.setPen(QPen(QBrush(Qt::red), mBounds.width() * 0.02));
	int startAngle = 90 * ARC_DRAWING_CONST;
	int spanAngle = constrain(mZ, MAX_GYROSCOPE_VAL, -MAX_GYROSCOPE_VAL) * ARC_DRAWING_CONST / MAX_GYROSCOPE_VAL * 180;
	painter.drawArc(mBounds, startAngle, spanAngle);

	mCircle.setPixmap(pixmap);
}
