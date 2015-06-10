#include "point.h"

using namespace trikControl;

Point::Point(int x, int y, QColor color, int penWidth)
	: Shape(color, penWidth)
	, mCoord(QPoint(x, y))
{
}

void Point::draw(QPainter *painter)
{
	painter->setPen(QPen(mColor, mPenWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
	painter->drawPoint(mCoord);
}

bool Point::equals(const Shape *other) const
{
	const Point *point = dynamic_cast<const Point *>(other);
	return point && mCoord == point->mCoord;
}
