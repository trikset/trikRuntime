/* Copyright 2014 Kogutich Denis, Smirnov Mikhail, CyberTech Labs Ltd.
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

#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtGui/QColor>

#include "include/trikControl/displayWidgetInterface.h"
#include "shapes/shape.h"

namespace trikControl {

/// Class of graphic widget.
class GraphicsWidget : public DisplayWidgetInterface
{
public:
	GraphicsWidget();
	~GraphicsWidget();

	/// Show this GraphicsWidget.
	void showCommand();

	/// Hide this GraphicsWidget.
	void hideCommand();

	/// Set painter color.
	void setPainterColor(const QColor &color);

	/// Set painter width.
	void setPainterWidth(int penWidth);

	/// Delete all items.
	void deleteAllItems();

	/// Delets only text labels.
	void deleteLabels();

	/// Draw point on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	void drawPoint(int x, int y);

	/// Draw line on the widget.
	/// @param x1 - first point's x coordinate.
	/// @param y1 - first point's y coordinate.
	/// @param x2 - second point's x coordinate.
	/// @param y2 - second point's y coordinate.
	void drawLine(int x1, int y1, int x2, int y2);

	/// Draw rect on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - rect's width.
	/// @param height - rect's height.
	/// @param filled - rect's filling.
	void drawRect(int x, int y, int width, int height, bool filled = false);

	/// Draw ellipse.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width of ellipse.
	/// @param height - height of ellipse.
	/// @param filled - filling of ellipse.
	void drawEllipse(int x, int y, int width, int height, bool filled = false);

	/// Draw arc on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width rect forming an arc.
	/// @param height - height rect forming an arc.
	/// @param startAngle - start angle.
	/// @param spanAngle - end angle.
	void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle);

	/// Prints text at given coordinates.
	void addLabel(const QString &text, int x, int y);

	/// Sets pixmap which will be drawn instead of other elements.
	void setPixmap(const QPixmap &picture);

private:
	/// Draw all elements.
	virtual void paintEvent(QPaintEvent *paintEvent);

	void addShape(Shape *shape);

	/// List of all labels.
	QHash<QPair<int, int>, QString> mLabels;

	QList<Shape *> mElements;

	QPixmap mPicture;

	/// Current pen color.
	QColor mCurrentPenColor;

	/// Current pen width.
	int mCurrentPenWidth;

	/// Font information used for printing text.
	QScopedPointer<QFontMetrics> mFontMetrics;
};

}
