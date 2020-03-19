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

#include "accelerometerWidget.h"

#include <QPainter>
#include <QString>
#include <cmath>
#include <QtMath>

#include "trikControl/vectorSensorInterface.h"

using namespace trikGui;

AccelerometerWidget::AccelerometerWidget(trikControl::VectorSensorInterface *accelerometer
		, QWidget *parent)
	: AbstractIndicator(parent)
	, mTitle(tr("Accelerometer"))
	, mAccelerometer(accelerometer)
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

void AccelerometerWidget::renew()
{
	const auto &value = mAccelerometer != nullptr ? mAccelerometer->read() : QVector<int>();

	const auto &text = [&value](int i) { return value.size() > i ? QString::number(value[i]) : tr("Error"); };

	mValueX.setText(QString("x: ") + text(0));
	mValueY.setText(QString("y: ") + text(1));
	mValueZ.setText(QString("z: ") + text(2));

	if (value.size() < 3) {
		return;
	}

	auto norm = qSqrt(value[0] * value[0] + value[1] * value[1] + value[2] * value[2]);
	QPointF acc(value[1] * mBounds.width() / norm, value[0] * mBounds.width() / norm);

	auto radius = (mBounds.width() - mKnopBounds.width()) / 2;
	auto smallNorm = qSqrt(QPointF::dotProduct(acc, acc));
	auto x = smallNorm > radius ? acc.x() * radius / smallNorm : acc.x();
	auto y = smallNorm > radius ? acc.y() * radius / smallNorm : acc.y();

	mKnopBounds.moveCenter(QPointF(x, y) + mBounds.center());
	update();
}

void AccelerometerWidget::resizeEvent(QResizeEvent *)
{
	auto size = qMin(mCircle.width(), mCircle.height());
	QPointF topleft((mCircle.width() - size) / 2, (mCircle.height() - size) / 2);

	mBounds = QRectF(topleft, QSize(size, size));
	mKnopBounds.setSize(QSizeF(size * 0.2, size * 0.2));
	mKnopBounds.moveCenter(mBounds.center());
}

void AccelerometerWidget::paintEvent(QPaintEvent *)
{
	QPixmap pixmap(mCircle.size());
	pixmap.fill(Qt::transparent);

	// draw big circle
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	painter.setPen(QPen(QBrush(Qt::black), mBounds.width() * 0.005));
	painter.setBrush(QBrush(Qt::transparent));
	painter.drawEllipse(mBounds);

	// draw crosshair
	painter.setPen(QColor(Qt::black));
	auto quarterLineLength = mBounds.width() * 0.35;
	painter.drawLine(QPointF(mBounds.left(), mBounds.center().y()),
				QPointF(mBounds.center().x() - quarterLineLength, mBounds.center().y()));

	painter.drawLine(QPointF(mBounds.center().x() + quarterLineLength, mBounds.center().y()),
				QPointF(mBounds.right(), mBounds.center().y()));

	painter.drawLine(QPointF(mBounds.center().x(), mBounds.top()),
				QPointF(mBounds.center().x(), mBounds.center().y() - quarterLineLength));

	painter.drawLine(QPointF(mBounds.center().x(), mBounds.center().y() + quarterLineLength),
				QPointF(mBounds.center().x(), mBounds.bottom()));

	painter.drawPoint(mBounds.center());

	// draw knob
	painter.setPen(QColor(Qt::darkGray));
	painter.setBrush(QBrush(Qt::darkGray));
	painter.drawEllipse(mKnopBounds);

	mCircle.setPixmap(pixmap);
}
