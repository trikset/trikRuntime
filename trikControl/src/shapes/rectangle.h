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

#include "shape.h"

namespace trikControl {

/// Presents a rectangle to draw on display.
class Rectangle : public Shape
{
public:
	/// Rectangle defined by top left corner (x,y), width and height.
	Rectangle(int x, int y, int width, int height, QColor color, int penWidth, bool filled);

	void draw(QPainter *painter) override;

	bool equals(const Shape *other) const override;

private:
	QRect mRect;
	bool mFilled;
};

}
