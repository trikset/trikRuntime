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
