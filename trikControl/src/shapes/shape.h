#pragma once

#include <QtGui/QPainter>

namespace trikControl {

class Shape
{
public:
	Shape(const QColor &color, int penWidth);
	virtual ~Shape();

	virtual void draw(QPainter *painter);

	virtual bool equals(const Shape *other) const;
protected:
	QColor mColor;
	int mPenWidth;
};

inline bool operator==(const Shape &first, const Shape &second)
{
	return first.equals(&second);
}

}