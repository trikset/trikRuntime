#include "ellipse.h"

using namespace trikControl;

Ellipse::Ellipse(int x, int y, int width, int height, QColor color, int penWidth)
	: Shape(color, penWidth)
	, mCenter(x, y)
	, mWidth(width)
	, mHeight(height)
{
}

void Ellipse::draw(QPainter *painter)
{
	painter->setPen(QPen(mColor, mPenWidth, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
	painter->drawEllipse(mCenter, mWidth, mHeight);
}

bool Ellipse::equals(const Shape *other) const
{
	const Ellipse *ellipse = dynamic_cast<const Ellipse *>(other);
	return ellipse && mCenter == ellipse->mCenter && mWidth == ellipse->mWidth && mHeight == ellipse->mHeight;
}
