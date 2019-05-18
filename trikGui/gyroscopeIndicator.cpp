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

#include <QtCore/QString>
#include <QtGui/QPainter>

#include <trikControl/gyroSensorInterface.h>

using namespace trikGui;

static constexpr int MAX_VAL = 250;

GyroscopeIndicator::GyroscopeIndicator(trikControl::GyroSensorInterface &gyroscope
		, QWidget *parent)
	: AbstractIndicator(parent)
	, mTitle(tr("Gyroscope"))
	, mGyroscope(gyroscope)
{
	mValueX.setAlignment(Qt::AlignVCenter);
	mValueY.setAlignment(Qt::AlignVCenter);
	mValueZ.setAlignment(Qt::AlignVCenter);

	mCircle.setAlignment(Qt::AlignVCenter);

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mValueX);
	mLayout.addWidget(&mValueY);
	mLayout.addWidget(&mValueZ);

	mLayout.addWidget(&mCircle);

	setLayout(&mLayout);
}

GyroscopeIndicator::~GyroscopeIndicator() {
	delete mPix;
}

template<typename T>
T constrain(T value, T max, T min)
{
	return (value > max) ? max : (value < min) ? min : value;
}

void GyroscopeIndicator::renew()
{
	QVector<int> value = mGyroscope.readRawData();

	mX = value[0];
	mY = value[1];
	mZ = value[2];

	mValueX.setText(QString("x: ") + QString::number(value[0]));
	mValueY.setText(QString("y: ") + QString::number(value[1]));
	mValueZ.setText(QString("z: ") + QString::number(value[2]));

	update();
}

void GyroscopeIndicator::resizeEvent(QResizeEvent *)
{
	mTitle.setFixedSize(width(), height() / 32);
	mValueX.setFixedSize(width(), height() / 32);
	mValueY.setFixedSize(width(), height() / 32);
	mValueZ.setFixedSize(width(), height() / 32);
	mCircle.setFixedSize(width(), height() * 11 / 16);

	mPix = new QPixmap(mCircle.width(), mCircle.height());

	float size = qMin(mCircle.width(), mCircle.height());

	QPointF topleft((mCircle.width() - size) / 2, (mCircle.height() - size) / 2);
	mBounds = QRectF(topleft, QSize(size, size));
}

void GyroscopeIndicator::paintEvent(QPaintEvent *)
{
	mPix->fill(Qt::transparent);

	QPainter painter(mPix);

	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	painter.setPen(QPen(QBrush(Qt::black), mBounds.width() * 0.003));
	painter.setBrush(QBrush(Qt::transparent));
	painter.drawEllipse(mBounds);

	// draw crosshair
	painter.setPen(QPen(QBrush(Qt::black), mBounds.width() * 0.001));
	painter.drawLine(QPointF(mBounds.left(), mBounds.center().y()),
					 QPointF(mBounds.right(), mBounds.center().y()));

	painter.drawLine(QPointF(mBounds.center().x(), mBounds.top()),
					 QPointF(mBounds.center().x(), mBounds.bottom()));

	// draw lines x, y, z
	painter.setPen(QPen(QBrush(Qt::blue), mBounds.width() * 0.02));
	painter.drawLine(QPointF(mBounds.center().x(), mBounds.center().y()),
					 QPointF(mBounds.center().x()
							 + constrain(mX, MAX_VAL, -MAX_VAL) * mBounds.width() / (2 * MAX_VAL),
							 mBounds.center().y()));

	painter.setPen(QPen(QBrush(Qt::green), mBounds.width() * 0.02));
	painter.drawLine(QPointF(mBounds.center().x(), mBounds.center().y()),
					 QPointF(mBounds.center().x(),
							 mBounds.center().y()
							 + constrain(mY, MAX_VAL, -MAX_VAL) * mBounds.width() / (2 * MAX_VAL)));

	int startAngle = 90 * 16;
	int spanAngle = constrain(mZ, MAX_VAL, -MAX_VAL) * 16 / MAX_VAL * 180;

	painter.setPen(QPen(QBrush(Qt::red), mBounds.width() * 0.02));
	painter.drawArc(mBounds, startAngle, spanAngle);

	mCircle.setPixmap(*mPix);
}
