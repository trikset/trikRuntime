#pragma once

#include "shape.h"

namespace trikControl {

class Rectangle : public Shape
{
public:
	Rectangle(int x, int y, int width, int height, QColor color, int penWidth);

	void draw(QPainter *painter) override;

	bool equals(const Shape *other) const override;

private:
	QRect mRect;
};

}
