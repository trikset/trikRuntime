#include "shape.h"

using namespace trikControl;

Shape::Shape(const QColor &color, int penWidth)
	: mColor(color)
	, mPenWidth(penWidth)
{
}

Shape::~Shape()
{
}

void Shape::draw(QPainter *painter)
{
	Q_UNUSED(painter)
}

bool Shape::equals(const Shape *other) const
{
	return other;
}
