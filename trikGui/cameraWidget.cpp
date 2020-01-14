#include "cameraWidget.h"

#include "trikControl/brickInterface.h"

using namespace trikGui;

CameraWidget::CameraWidget(trikControl::BrickInterface &brick, QWidget *parent)
	: AbstractIndicator(parent)
	, mTitle(tr("Camera"))
	, mBrick(brick)
{
	mTitle.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	mPixmap.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mPixmap);

	setLayout(&mLayout);

	auto const & photo = mBrick.getStillImage();
	if (!photo.isEmpty()) {
		mPixmap.setPixmap(QPixmap::fromImage(QImage(photo.data(), 320, 240, QImage::Format_RGB888)));
	} else {
		mPixmap.setText(tr("Camera is not available"));
	}
}

void CameraWidget::renew()
{
	update();
}
