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

#include "arc.h"

using namespace trikControl;

Arc::Arc(int x, int y, int width, int height, int startAngle, int spanAngle, QColor color, int penWidth)
	: Shape(color, penWidth)
	, mArc(QRect(x, y, width, height))
	, mStartAngle(startAngle)
	, mSpanAngle(spanAngle)
{
}

void Arc::draw(QPainter *painter)
{
	painter->setPen(QPen(mColor, mPenWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
	painter->drawArc(mArc, mStartAngle, mSpanAngle);
}

bool Arc::equals(const Shape *other) const
{
	const Arc *arc = dynamic_cast<const Arc *>(other);
	return arc && mArc == arc->mArc && mSpanAngle == arc->mSpanAngle && mStartAngle == arc->mStartAngle;
}
