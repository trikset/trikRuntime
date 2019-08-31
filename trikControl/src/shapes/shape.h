/* Copyright 2015 CyberTech Labs ltd.
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

#include <QtGui/QPainter>

namespace trikControl {

/// Base class for elements that can be drawn on display.
class Shape
{
	Q_DISABLE_COPY(Shape)
public:
	/// @param color - color of pen with which a shape must be drawn
	/// @param penWidth - width of lines
	Shape(const QColor &color, int penWidth)
		: mColor(color)
		, mPenWidth(penWidth)
	{
	}

	virtual ~Shape() = default;

	/// Draws a shape with a given painter.
	virtual void draw(QPainter *painter) = 0;

	/// Checks whether to shapes are equal.
	virtual bool equals(const Shape *other) const = 0;

protected:
	QColor mColor;
	int mPenWidth;
};

}
