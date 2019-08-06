#pragma once

#include "abstractIndicator.h"

namespace trikControl {
class BrickInterface;
}

namespace trikGui {

class CameraWidget : public AbstractIndicator
{
	Q_OBJECT
public:
	CameraWidget(trikControl::BrickInterface &brick, QWidget *parent = nullptr);

public slots:
	void renew() override;

private:
	void paintEvent(QPaintEvent *) override;

	QVBoxLayout mLayout;
	QLabel mTitle;
	QLabel mPixmap;
	trikControl::BrickInterface &mBrick;
};

}
