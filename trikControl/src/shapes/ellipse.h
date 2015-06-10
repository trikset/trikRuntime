#pragma once

#include "shape.h"

namespace trikControl {

class Ellipse : public Shape
{
public:
	Ellipse(int x, int y, int width, int height, QColor color, int penWidth);

	void draw(QPainter *painter) override;

	bool equals(const Shape *other) const;

private:
	QPoint mCenter;
	int mWidth;
	int mHeight;
};

}