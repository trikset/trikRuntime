/* Copyright 2016 Mikhail Kita
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

#include "sensorLever.h"

#include <QtGui/QKeyEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QStylePainter>
	#include <QtGui/QStyleOptionFocusRect>
#else
	#include <QtWidgets/QStylePainter>
	#include <QtWidgets/QStyleOptionFocusRect>
#endif

using namespace trikGui;

SensorLever::SensorLever(const QString &port, QWidget *parent)
	: QWidget(parent)
	, mIsGreater(true)
	, mMaxDistance(100)
	, mMinDistance(0)
	, mDistanceStep(5)
	, mDistance(0)
	, mNameLabel(port)
	, mSignLabel(QString(">"))
{
	mDistanceBar.setOrientation(Qt::Horizontal);
	mDistanceBar.setMinimum(mMinDistance);
	mDistanceBar.setMaximum(mMaxDistance);
	mDistanceBar.setValue(0);
	mDistanceBar.setTextVisible(true);
	mDistanceBar.setFormat("%v");

	mNameLabel.setAlignment(Qt::AlignCenter);
	mDistanceBar.setAlignment(Qt::AlignCenter);
	mSignLabel.setAlignment(Qt::AlignCenter);

	// mSignLabel can change its width during work. It will cause mDistanceBar
	// width change. To prevent it, we set fixed width it.
	mSignLabel.setFixedWidth(48);

	mLayout.addWidget(&mNameLabel);
	mLayout.addWidget(&mDistanceBar);
	mLayout.addWidget(&mSignLabel);
	setLayout(&mLayout);

	setFocusPolicy(Qt::StrongFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::QSizePolicy::MinimumExpanding);
}

void SensorLever::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Right: {
		setDistance(mDistance + mDistanceStep);
		break;
	}
	case Qt::Key_Left: {
		setDistance(mDistance - mDistanceStep);
		break;
	}
	case Qt::Key_Return: {
		changeSign();
		break;
	}
	default: {
		QWidget::keyPressEvent(event);
	}
	}
}

void SensorLever::paintEvent(QPaintEvent *)
{
	QStylePainter painter(this);

	if (hasFocus()) {
		QStyleOptionFocusRect option;
		option.initFrom(this);
		option.backgroundColor = palette().color(QPalette::Background);

		painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
	}
}

void SensorLever::setDistance(int distance)
{
	if (distance > mMaxDistance || distance < mMinDistance) {
		return;
	}

	mDistance = distance;
	mDistanceBar.setValue(distance);
}

void SensorLever::changeSign()
{
	mIsGreater = !mIsGreater;
	mIsGreater ? mSignLabel.setText(QString(">")) : mSignLabel.setText(QString("<"));
}

int SensorLever::distance() const
{
	return mDistance;
}

bool SensorLever::isGreater() const
{
	return mIsGreater;
}
