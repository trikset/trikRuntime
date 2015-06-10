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
