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

#include "line.h"

using namespace trikControl;

Line::Line(int x1, int y1, int x2, int y2, QColor color, int penWidth)
	: Shape(color, penWidth)
	, mCoord1(QPoint(x1, y1))
	, mCoord2(QPoint(x2, y2))
{
}

void Line::draw(QPainter *painter)
{
	painter->setPen(QPen(mColor, mPenWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
	painter->drawLine(mCoord1, mCoord2);
}

bool Line::equals(const Shape *other) const
{
	const Line *line = dynamic_cast<const Line *>(other);
	return line && line->mCoord1 == mCoord1 && line->mCoord2 == mCoord2;
}
