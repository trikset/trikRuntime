/* Copyright 2014 - 2015 Kogutich Denis, Smirnov Mikhail and CyberTech Labs ltd.
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

#include "shapes/arc.h"
#include "shapes/ellipse.h"
#include "shapes/line.h"
#include "shapes/point.h"
#include "shapes/rectangle.h"

using namespace trikControl;

GraphicsWidget::GraphicsWidget()
{
	setAutoFillBackground(true);
}

GraphicsWidget::~GraphicsWidget()
{
	qDeleteAll(mElements);
}

void GraphicsWidget::showCommand()
{
	show();
	emit shown();
}

void GraphicsWidget::hideCommand()
{
	hide();
	emit hidden();
}

void GraphicsWidget::paintEvent(QPaintEvent *paintEvent)
{
	Q_UNUSED(paintEvent)

	QPainter painter(this);

	if (!mPicture.isNull()) {
		painter.drawPixmap(geometry(), mPicture);
	}

	for (Shape *shape : mElements) {
		shape->draw(&painter);
	}

	for (const auto & position : mLabels.keys()) {
		painter.setPen(mCurrentPenColor);
		auto & textObject = mLabels[position];
		QFontMetrics fontMetrics(textObject.font);
		painter.setPen(textObject.currentPenColor);
		painter.setFont(textObject.font);
		painter.drawText(position.first, position.second
#if (QT_VERSION_MAJOR == 5) && (QT_VERSION_MINOR < 11)
				, fontMetrics.width(textObject.text)
#else
				, fontMetrics.horizontalAdvance(textObject.text)
#endif
				, fontMetrics.height()
				, Qt::TextWordWrap, textObject.text);
	}
}

void GraphicsWidget::deleteAllItems()
{
	qDeleteAll(mElements);
	mElements.clear();
	deleteLabels();
	mPicture = QPixmap();
}

void GraphicsWidget::deleteLabels()
{
	mLabels.clear();
}

void GraphicsWidget::setPainterColor(const QColor &color)
{
	mCurrentPenColor = color;
}

void GraphicsWidget::setPainterWidth(int penWidth)
{
	mCurrentPenWidth = penWidth;
}

void GraphicsWidget::drawPoint(int x, int y)
{
	addShape(new Point(x, y, mCurrentPenColor, mCurrentPenWidth));
}

void GraphicsWidget::drawLine(int x1, int y1, int x2, int y2)
{
	addShape(new Line(x1, y1, x2, y2, mCurrentPenColor, mCurrentPenWidth));
}

void GraphicsWidget::drawRect(int x, int y, int width, int height, bool filled)
{
	addShape(new Rectangle(x, y, width, height, mCurrentPenColor, mCurrentPenWidth, filled));
}

void GraphicsWidget::drawEllipse(int x, int y, int width, int height, bool filled)
{
	addShape(new Ellipse(x, y, width, height, mCurrentPenColor, mCurrentPenWidth, filled));
}

void GraphicsWidget::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	addShape(new Arc(x, y, width, height, startAngle, spanAngle, mCurrentPenColor, mCurrentPenWidth));
}

void GraphicsWidget::addShape(Shape *shape)
{
	auto found = std::find_if(mElements.begin(), mElements.end(), [=](Shape *x) { return x->equals(shape);});
	if (found == mElements.end())
		mElements << shape;
	else *found = shape;
}

void GraphicsWidget::addLabel(const QString &text, int x, int y, int fontSize)
{
	TextObject newTextObject(text, mCurrentPenColor, fontSize);
	mLabels[qMakePair(x, y)] = newTextObject;
}

void GraphicsWidget::setPixmap(QPixmap &&picture)
{
	mPicture = picture;
}
