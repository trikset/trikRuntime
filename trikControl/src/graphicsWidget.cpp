/* Copyright 2014 Kogutich Denis, Smirnov Mikhail
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
#include <QtGui/QPen>
#include <QtGui/QKeyEvent>

#include "graphicsWidget.h"

using namespace trikControl;

GraphicsWidget::GraphicsWidget()
	: trikKernel::LazyMainWidget()
	, mCurrentPenColor(Qt::black)
	, mCurrentPenWidth(0)
{
}

void GraphicsWidget::showCommand()
{
	emit showMe(*this);
}

void GraphicsWidget::hideCommand()
{
	emit hideMe();
}

void GraphicsWidget::renewFocus()
{
	setFocus();
}

void GraphicsWidget::paintEvent(QPaintEvent *paintEvent)
{
	Q_UNUSED(paintEvent)

	QPainter painter(this);

	for (int i = 0; i < mLines.length(); i++)
	{
		painter.setPen(QPen(mLines.at(i).color, mLines.at(i).penWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
		painter.drawLine(mLines.at(i).coord1.x(), mLines.at(i).coord1.y()
				, mLines.at(i).coord2.x(), mLines.at(i).coord2.y());
	}

	for (int i = 0; i < mPoints.length(); i++)
	{
		painter.setPen(QPen(mPoints.at(i).color, mPoints.at(i).penWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
		painter.drawPoint(mPoints.at(i).coord.x(), mPoints.at(i).coord.y());
	}

	for (int i = 0; i < mRects.length(); i++)
	{
		painter.setPen(QPen(mRects.at(i).color, mRects.at(i).penWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
		painter.drawRect(mRects.at(i).rect.x(), mRects.at(i).rect.y()
				, mRects.at(i).rect.width(), mRects.at(i).rect.height());
	}

	for (int i = 0; i < mEllipses.length(); i++)
	{
		painter.setPen(QPen(mEllipses.at(i).color, mEllipses.at(i).penWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
		painter.drawEllipse(mEllipses.at(i).ellipse.x(), mEllipses.at(i).ellipse.y()
				, mEllipses.at(i).ellipse.width(), mEllipses.at(i).ellipse.height());
	}

	for (int i = 0; i < mArcs.length(); i++)
	{
		painter.setPen(QPen(mArcs.at(i).color, mArcs.at(i).penWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
		painter.drawArc(mArcs.at(i).arc.x(), mArcs.at(i).arc.y()
				, mArcs.at(i).arc.width(), mArcs.at(i).arc.height()
				, mArcs.at(i).startAngle, mArcs.at(i).spanAngle);
	}
}

void GraphicsWidget::deleteAllItems()
{
	mPoints.clear();
	mLines.clear();
	mRects.clear();
	mEllipses.clear();
	mArcs.clear();
}

void GraphicsWidget::setPainterColor(QString const &color)
{
	if (color == tr("white")) {
		mCurrentPenColor = Qt::white;
	} else if (color == tr("red")) {
		mCurrentPenColor = Qt::red;
	} else if (color == tr("darkRed")) {
		mCurrentPenColor = Qt::darkRed;
	} else if (color == tr("green")) {
		mCurrentPenColor = Qt::green;
	} else if (color == tr("darkGreen")) {
		mCurrentPenColor = Qt::darkGreen;
	} else if (color == tr("blue")) {
		mCurrentPenColor = Qt::blue;
	} else if (color == tr("darkBlue")) {
		mCurrentPenColor = Qt::darkBlue;
	} else if (color == tr("cyan")) {
		mCurrentPenColor = Qt::cyan;
	} else if (color == tr("darkCyan")) {
		mCurrentPenColor = Qt::darkCyan;
	} else if (color == tr("magenta")) {
		mCurrentPenColor = Qt::magenta;
	} else if (color == tr("darkMagenta")) {
		mCurrentPenColor = Qt::darkMagenta;
	} else if (color == tr("yellow")) {
		mCurrentPenColor = Qt::yellow;
	} else if (color == tr("darkYellow")) {
		mCurrentPenColor = Qt::darkYellow;
	} else if (color == tr("gray")) {
		mCurrentPenColor = Qt::gray;
	} else if (color == tr("darkGray")) {
		mCurrentPenColor = Qt::darkGray;
	} else if (color == tr("lightGray")) {
		mCurrentPenColor = Qt::lightGray;
	} else {
		mCurrentPenColor = Qt::black;
	}
}

void GraphicsWidget::setPainterWidth(int penWidth)
{
	mCurrentPenWidth = penWidth;
}

void GraphicsWidget::drawPoint(int x, int y)
{
	PointCoordinates const coordinates(x, y, mCurrentPenColor, mCurrentPenWidth);

	if (!containsPoint(coordinates)) {
		mPoints.insert(mPoints.length(), coordinates);
	}
}

bool GraphicsWidget::containsPoint(PointCoordinates const &coordinates)
{
	for (int i = 0; i < mPoints.length(); ++i) {
		if (mPoints.at(i).coord.x() == coordinates.coord.x()
			&& mPoints.at(i).coord.y() == coordinates.coord.y())
		{
			return true;
		}
	}

	return false;
}

void GraphicsWidget::drawLine(int x1, int y1, int x2, int y2)
{
	LineCoordinates const coordinates(x1, y1, x2, y2, mCurrentPenColor, mCurrentPenWidth);

	if (!containsLine(coordinates)) {
		mLines.insert(mLines.length(), coordinates);
	}
}

bool GraphicsWidget::containsLine(LineCoordinates const &coordinates)
{
	for (int i = 0; i < mLines.length(); ++i) {
		if (mLines.at(i).coord1.x() == coordinates.coord1.x()
			&& mLines.at(i).coord1.y() == coordinates.coord1.y()
			&& mLines.at(i).coord2.x() == coordinates.coord2.x()
			&& mLines.at(i).coord2.y() == coordinates.coord2.y())
		{
			return true;
		}
	}

	return false;
}

void GraphicsWidget::drawRect(int x, int y, int width, int height)
{
	RectCoordinates const coordinates(x, y, width, height, mCurrentPenColor, mCurrentPenWidth);

	if (!containsRect(coordinates)) {
		mRects.insert(mRects.length(), coordinates);
	}
}

bool GraphicsWidget::containsRect(RectCoordinates const &coordinates)
{
	for (int i = 0; i < mRects.length(); ++i) {
		if (mRects.at(i).rect.x() == coordinates.rect.x()
			&& mRects.at(i).rect.y() == coordinates.rect.y()
			&& mRects.at(i).rect.width() == coordinates.rect.width()
			&& mRects.at(i).rect.height() == coordinates.rect.height())
		{
			return true;
		}
	}

	return false;
}

void GraphicsWidget::drawEllipse(int x, int y, int width, int height)
{
	EllipseCoordinates const coordinates(x, y, width, height, mCurrentPenColor, mCurrentPenWidth);

	if (!containsEllipse(coordinates)) {
		mEllipses.insert(mEllipses.length(), coordinates);
	}
}

bool GraphicsWidget::containsEllipse(EllipseCoordinates const &coordinates)
{
	for (int i = 0; i < mEllipses.length(); i++) {
		if (mEllipses.at(i).ellipse.x() == coordinates.ellipse.x()
			&& mEllipses.at(i).ellipse.y() == coordinates.ellipse.y()
			&& mEllipses.at(i).ellipse.width() == coordinates.ellipse.width()
			&& mEllipses.at(i).ellipse.height() == coordinates.ellipse.height())
		{
			return true;
		}
	}

	return false;
}

void GraphicsWidget::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	ArcCoordinates const coordinates(x, y, width, height, startAngle, spanAngle, mCurrentPenColor, mCurrentPenWidth);

	if (!containsArc(coordinates)) {
		mArcs.insert(mArcs.length(), coordinates);
	}
}

bool GraphicsWidget::containsArc(ArcCoordinates const &coordinates)
{
	for (int i = 0; i < mArcs.length(); ++i) {
		if (mArcs.at(i).arc.x() == coordinates.arc.x()
			&& mArcs.at(i).arc.y() == coordinates.arc.y()
			&& mArcs.at(i).arc.width() == coordinates.arc.width()
			&& mArcs.at(i).arc.height() == coordinates.arc.height()
			&& mArcs.at(i).startAngle == coordinates.startAngle
			&& mArcs.at(i).spanAngle == coordinates.spanAngle)
		{
			return true;
		}
	}

	return false;
}

QColor GraphicsWidget::currentPenColor() const
{
	return mCurrentPenColor;
}
