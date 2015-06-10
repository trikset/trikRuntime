#pragma once

#include "shape.h"

namespace trikControl {

class Point : public Shape
{
public:
	Point(int x, int y, QColor color, int penWidth);

	void draw(QPainter *painter) override;

	bool equals(const Shape *other) const;

private:
	QPoint mCoord;
};

}
