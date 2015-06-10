#include "rectangle.h"

using namespace trikControl;

Rectangle::Rectangle(int x, int y, int width, int height, QColor color, int penWidth)
	: Shape(color, penWidth)
	, mRect(QRect(x, y, width, height))
{
}

void Rectangle::draw(QPainter *painter)
{
	painter->setPen(QPen(mColor, mPenWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
	painter->drawRect(mRect);
}

bool Rectangle::equals(const Shape *other) const
{
	const Rectangle *rect = dynamic_cast<const Rectangle *>(other);
	return rect && mRect == rect->mRect;
}
