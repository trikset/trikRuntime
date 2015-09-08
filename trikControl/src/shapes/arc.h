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

/// Presents an arc to draw on display.
class Arc : public Shape
{
public:
	/// Arc in rectangle defined by (x,y,width,height), with given start and span angles.
	Arc(int x, int y, int width, int height, int startAngle, int spanAngle, QColor color, int penWidth);

	void draw(QPainter *painter) override;

	bool equals(const Shape *other) const override;

private:
	QRect mArc;
	int mStartAngle;
	int mSpanAngle;
};

}
