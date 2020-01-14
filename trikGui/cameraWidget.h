#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

#include "abstractIndicator.h"

namespace trikControl {
class BrickInterface;
}

namespace trikGui {

class CameraWidget : public AbstractIndicator
{
	Q_OBJECT
public:
	explicit CameraWidget(trikControl::BrickInterface &brick, QWidget *parent = nullptr);

public slots:
	void renew() override;

private:
	QVBoxLayout mLayout;
	QLabel mTitle;
	QLabel mPixmap;
	trikControl::BrickInterface &mBrick;
};

}
