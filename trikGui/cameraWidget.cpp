/* Copyright 2019-2020 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "cameraWidget.h"

#include <QDir>
#include <QtGui/QKeyEvent>

#include "trikControl/brickInterface.h"
#include "trikControl/utilities.h"

#include "trikKernel/paths.h"
#include "QsLog.h"

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

	doPhoto();
}

void CameraWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return: {
			doPhoto();
			break;
		}
		default: {
			event->ignore();
			break;
		}
	}
}

void CameraWidget::renew()
{
	update();
}

void CameraWidget::doPhoto()
{
	if (mIsCreatingPhoto.exchange(true)) {
		return;
	}

	auto const photo = trikControl::Utilities::rescalePhoto(mBrick.getStillImage());
	auto image = trikControl::Utilities::imageFromBytes(photo, 160, 120, "rgb32");

	if (!image.isNull()) {
		mPixmap.setPixmap(QPixmap::fromImage(image));
		QDir dir(trikKernel::Paths::imagesPath());

		if (!dir.exists() && !dir.mkpath(trikKernel::Paths::imagesPath())) {
			QLOG_ERROR() << "Cannot create directory for images";
		} else {
			const auto & name = trikKernel::Paths::imagesPath() + "/photo_" + QString::number(dir.count() - 1) + ".jpg";
			if (!image.save(name, "JPG")) {
				QLOG_ERROR() << "Failed to save captured image" << name;
			}
		}
	} else {
		mPixmap.setText(tr("Camera is not available"));
	}
	update();

	mIsCreatingPhoto.store(false);
}
