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

#include <QtGui/QPainter>
#include <QtCore/QString>
#include <cmath>
#include "accelerometerIndicator.h"
#include "trikControl/vectorSensorInterface.h"

using namespace trikGui;

AccelerometerIndicator::AccelerometerIndicator(trikControl::VectorSensorInterface &accelerometer
		, QWidget *parent)
	: AbstractIndicator(parent)
	, mTitle(tr("Accelerometer"))
	, mAccelerometer(accelerometer)
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

AccelerometerIndicator::~AccelerometerIndicator() {
	delete mPix;
}

template<typename T>
T constrain(T value1, T value2, T max)
{
	T norm = sqrtf(value1 * value1 + value2 * value2);
	return (norm > max) ? (value1 * max / norm) : value1;
}

void AccelerometerIndicator::renew()
{
	QVector<int> value = mAccelerometer.read();

	mValueX.setText(QString("x: ") + QString::number(value[0]));
	mValueY.setText(QString("y: ") + QString::number(value[1]));
	mValueZ.setText(QString("z: ") + QString::number(value[2]));


	float norm = sqrtf(value[0] * value[0] + value[1] * value[1] + value[2] * value[2]);

	QPointF acc(value[1] / norm * mBounds.width(), value[0] / norm * mBounds.width());

	qreal radius = (mBounds.width() - mKnopBounds.width()) / 2;

	acc.setX(constrain(acc.x(), acc.y(), radius));
	acc.setY(constrain(acc.y(), acc.x(), radius));

	mKnopBounds.moveCenter(acc + mBounds.center());
	update();
}


void AccelerometerIndicator::resizeEvent(QResizeEvent *)
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

	mKnopBounds.setWidth(size * 0.3);
	mKnopBounds.setHeight(size * 0.3);

	// adjust knob position
	mKnopBounds.moveCenter(QPointF(mBounds.center().x(), mBounds.center().y()));
}

void AccelerometerIndicator::paintEvent(QPaintEvent *)
{
	mPix->fill(Qt::transparent);
	QPainter painter(mPix);

	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// draw big circle
	painter.setPen(QPen(QBrush(Qt::black), mBounds.width() * 0.005));
	painter.setBrush(QBrush(Qt::transparent));
	painter.drawEllipse(mBounds);

	// draw crosshair
	painter.setPen(QPen(QBrush(Qt::black), mBounds.width() * 0.005));
	painter.drawLine(QPointF(mBounds.left(), mBounds.center().y()),
					 QPointF(mBounds.center().x() - mBounds.width() * 0.35, mBounds.center().y()));

	painter.drawLine(QPointF(mBounds.center().x() + mBounds.width() * 0.35, mBounds.center().y()),
					 QPointF(mBounds.right(), mBounds.center().y()));

	painter.drawLine(QPointF(mBounds.center().x(), mBounds.top()),
					 QPointF(mBounds.center().x(), mBounds.center().y() - mBounds.width() * 0.35));

	painter.drawLine(QPointF(mBounds.center().x(), mBounds.center().y() + mBounds.width() * 0.35),
					 QPointF(mBounds.center().x(), mBounds.bottom()));

	painter.drawPoint(mBounds.center().x(), mBounds.center().y());

	// draw knob
	painter.setPen(QPen(QBrush(Qt::darkGray), mBounds.width() * 0.005));
	painter.setBrush(QBrush(Qt::darkGray));
	painter.drawEllipse(mKnopBounds);

	mCircle.setPixmap(*mPix);
}
