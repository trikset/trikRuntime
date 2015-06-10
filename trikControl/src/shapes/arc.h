#pragma once

#include "shape.h"

namespace trikControl {

class Arc : public Shape
{
public:
	Arc(int x, int y, int width, int height, int startAngle, int spanAngle, QColor color, int penWidth);

	void draw(QPainter *painter) override;

	bool equals(const Shape *other) const;

private:
	QRect mArc;
	int mStartAngle;
	int mSpanAngle;
};

}
