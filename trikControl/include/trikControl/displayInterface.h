/* Copyright 2015 CyberTech Labs Ltd.
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

#include <QtCore/QObject>
#include <QtCore/QString>

#include "displayWidgetInterface.h"

#include "declSpec.h"

namespace trikControl {

/// Provides ability to draw something on robot display.
class TRIKCONTROL_EXPORT DisplayInterface : public QObject
{
	Q_OBJECT

public:
	/// Returns widget on which everything is drawn.
	virtual DisplayWidgetInterface &graphicsWidget() = 0;

public slots:
	/// Shows given image on a display.
	/// @param fileName - file name (with path) of an image to show. Refer to Qt documentation for
	///        supported formats, but .jpg, .png, .bmp, .gif are supported.
	virtual void showImage(const QString &fileName) = 0;

	/// Add a label to the specific position of the screen without redrawing it.
	/// If there already is a label in these coordinates, its contents will be updated.
	/// @param text - label text.
	/// @param x - label x coordinate.
	/// @param y - label y coordinate.
	virtual void addLabel(const QString &text, int x, int y) = 0;

	/// Remove all labels from the screen.
	virtual void removeLabels() = 0;

	/// Set painter color.
	virtual void setPainterColor(const QString &color) = 0;

	/// Set painter width.
	virtual void setPainterWidth(int penWidth) = 0;

	/// Draw line on the widget without redrawing it.
	/// @param x1 - first point's x coordinate.
	/// @param y1 - first point's y coordinate.
	/// @param x2 - second point's x coordinate.
	/// @param y2 - second point's y coordinate.
	virtual void drawLine(int x1, int y1, int x2, int y2) = 0;

	/// Draw point on the widget without redrawing it.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	virtual void drawPoint(int x, int y) = 0;

	/// Draw rect on the widget without redrawing it.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - rect's width.
	/// @param height - rect's height.
	/// @param filled - rect's filling.
	virtual void drawRect(int x, int y, int width, int height, bool filled = false) = 0;

	/// Draw ellipse without redrawing display.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width of ellipse.
	/// @param height - height of ellipse.
	/// @param filled - filling of ellipse.
	virtual void drawEllipse(int x, int y, int width, int height, bool filled = false) = 0;

	/// Draw arc on the widget without redrawing it without redrawing it.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width rect forming an arc.
	/// @param height - height rect forming an arc.
	/// @param startAngle - start angle.
	/// @param spanAngle - end andle.
	virtual void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle) = 0;

	/// Sets background for a picture.
	/// @param color - color of a background.
	virtual void setBackground(const QString &color) = 0;

	/// Hides and clears widget on which everything is drawn.
	virtual void hide() = 0;

	/// Clear everything painted with this object.
	virtual void clear() = 0;

	/// Clears screen, returns a display in a blank state.
	virtual void reset() = 0;

	/// Updates painted picture on the robot`s screen.
	/// @warning This operation is pretty slow, so it shouldn`t be called without need.
	virtual void redraw() = 0;
};

}
