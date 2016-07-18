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

#include "rectangle.h"

using namespace trikControl;

Rectangle::Rectangle(int x, int y, int width, int height, QColor color, int penWidth, bool filled)
	: Shape(color, penWidth)
	, mRect(QRect(x, y, width, height))
	, mFilled(filled)
{
}

void Rectangle::draw(QPainter *painter)
{
	painter->setPen(QPen(mColor, mPenWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
	painter->drawRect(mRect);
	if (mFilled) {
		painter->fillRect(mRect, mColor);
	}
}

bool Rectangle::equals(const Shape *other) const
{
	const Rectangle *rect = dynamic_cast<const Rectangle *>(other);
	return rect && mRect == rect->mRect && mFilled == rect->mFilled;
}
