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

#include "ellipse.h"

using namespace trikControl;

Ellipse::Ellipse(int x, int y, int width, int height, QColor color, int penWidth, bool filled)
	: Shape(color, penWidth)
	, mCenter(x, y)
	, mWidth(width)
	, mHeight(height)
	, mFilled(filled)
{
}

void Ellipse::draw(QPainter *painter)
{
	painter->setPen(QPen(mColor, mPenWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
	if (mFilled) {
		const QBrush painterBrush = painter->brush();
		painter->setBrush(mColor);
		painter->drawEllipse(mCenter, mWidth, mHeight);
		painter->setBrush(painterBrush);
	} else {
		painter->drawEllipse(mCenter, mWidth, mHeight);
	}
}

bool Ellipse::equals(const Shape *other) const
{
	const Ellipse *ellipse = dynamic_cast<const Ellipse *>(other);
	return ellipse && mCenter == ellipse->mCenter && mWidth == ellipse->mWidth && mHeight == ellipse->mHeight;
}
