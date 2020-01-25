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

#include "trikControl/brickInterface.h"
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

	auto const & photo = mBrick.getStillImage();

	if (!photo.isEmpty()) {
		const auto & image = QImage(photo.data(), 320, 240, QImage::Format_RGB888);
		mPixmap.setPixmap(QPixmap::fromImage(image));

		QDir dir(trikKernel::Paths::imagesPath());

		if (!dir.exists()) {
			if (!dir.mkpath(trikKernel::Paths::imagesPath())) {
				QLOG_ERROR() << "Can not create directory for images";
			}
		} else {
			const auto & name = trikKernel::Paths::imagesPath() + "/photo_" + QString::number(dir.count() - 1) + ".jpg";
			if (!image.save(name, "JPG")) {
				QLOG_ERROR() << "Failed to save captured image";
			}
		}
	} else {
		mPixmap.setText(tr("Camera is not available"));
	}
}

void CameraWidget::renew()
{
	update();
}
