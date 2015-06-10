#pragma once

#include "shape.h"

namespace trikControl {

class Line : public Shape
{
public:
	Line(int x1, int y1, int x2, int y2, QColor color, int penWidth);

	void draw(QPainter *painter) override;

	bool equals(const Shape *other) const;

private:
	QPoint mCoord1;
	QPoint mCoord2;
};

}
