#pragma once

#include <QtGui/QPainter>

namespace trikControl {

class Shape
{
public:
	Shape(const QColor &color, int penWidth)
		: mColor(color)
		, mPenWidth(penWidth)
	{
	}

	virtual ~Shape() {}

	virtual void draw(QPainter *painter) = 0;

	virtual bool equals(const Shape *other) const = 0;

protected:
	QColor mColor;
	int mPenWidth;
};

}