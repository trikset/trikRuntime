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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
#else
	#include <QtWidgets/QWidget>
#endif

namespace trikControl {

/// Class of graphic widget.
class GraphicsWidget : public QWidget
{
public:
	GraphicsWidget();

	/// Set painter color.
	void setPainterColor(QString const &color);

	/// Set painter width.
	void setPainterWidth(int penWidth);

	/// Delete all items.
	void deleteAllItems();

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
	void drawRect(int x, int y, int width, int height);

	/// Draw ellipse.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width of ellipse.
	/// @param height - height of ellipse.
	void drawEllipse(int x, int y, int width, int height);

	/// Draw arc on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width rect forming an arc.
	/// @param height - height rect forming an arc.
	/// @param startAngle - start angle.
	/// @param spanAngle - end angle.
	void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle);

	/// Returns current pen color.
	QColor currentPenColor() const;

private:
	/// Information about point.
	struct PointCoordinates
	{
		PointCoordinates(int x, int y, QColor color, int penWidth)
				: coord(QPoint(x, y)), color(color), penWidth(penWidth)
		{
		}

		QPoint coord;
		QColor color;
		int penWidth;
	};

	/// Information about rectangle.
	struct RectCoordinates
	{
		RectCoordinates(int x, int y, int width, int height, QColor color, int penWidth)
			: rect(QRect(x, y, width, height)), color(color), penWidth(penWidth)
		{
		}

		QRect rect;
		QColor color;
		int penWidth;
	};

	/// Information about line.
	struct LineCoordinates
	{
		LineCoordinates(int x1, int y1, int x2, int y2, QColor color, int penWidth)
			: coord1(QPoint(x1, y1)), coord2(QPoint(x2, y2)), color(color), penWidth(penWidth)
		{
		}

		QPoint coord1;
		QPoint coord2;
		QColor color;
		int penWidth;
	};

	/// Struct of ellipse coordinates.
	struct EllipseCoordinates
	{
		EllipseCoordinates(int x, int y, int width, int height, QColor color, int penWidth)
			: ellipse(QRect(x, y, width, height)), color(color), penWidth(penWidth)
		{
		}

		QRect ellipse;
		QColor color;
		int penWidth;
	};

	/// Struct of arc coordinates.
	struct ArcCoordinates
	{
		ArcCoordinates(int x, int y, int width, int height, int startAngle, int spanAngle, QColor color, int penWidth)
			: arc(QRect(x, y, width, height))
			, startAngle(startAngle)
			, spanAngle(spanAngle)
			, color(color)
			, penWidth(penWidth)
		{
		}

		QRect arc;
		int startAngle;
		int spanAngle;
		QColor color;
		int penWidth;
	};

	/// Draw all elements.
	virtual void paintEvent(QPaintEvent *paintEvent);

	/// Check list contains point.
	/// @param coordinates - point that we are looking for.
	/// @return - true if list contains this point.
	bool containsPoint(PointCoordinates const &coordinates);

	/// Check that list contains given line.
	/// @param coordinates - line that we are looking for.
	/// @return - true if list contains this line.
	bool containsLine(LineCoordinates const &coordinates);

	/// Check that list contains given rect.
	/// @param coordinates - rect that we are looking for.
	/// @return - true if list contains this rect.
	bool containsRect(RectCoordinates const &coordinates);

	/// Check that list contains given ellipse.
	/// @param coordinates - ellipse that we are looking for.
	/// @return - true if list contains this ellipse.
	bool containsEllipse(EllipseCoordinates const &coordinates);

	/// Check that list contains given arc.
	/// @param coordinates - arc that we are looking for.
	/// @return - true if list contains this arc.
	bool containsArc(ArcCoordinates const &coordinates);

	/// List of all lines.
	QList<LineCoordinates> mLines;

	/// List of all points.
	QList<PointCoordinates> mPoints;

	/// List of all rectangles.
	QList<RectCoordinates> mRects;

	/// List of all ellipses.
	QList<EllipseCoordinates> mEllipses;

	/// List of all arcs.
	QList<ArcCoordinates> mArcs;

	/// Current pen color.
	QColor mCurrentPenColor;

	/// Current pen width.
	int mCurrentPenWidth;
};

}
