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

#include "camera.h"

#include <QDir>

#include "trikControl/brickInterface.h"
#include "trikControl/utilities.h"

#include "trikKernel/paths.h"
#include "QsLog.h"

using namespace trikGui;

Camera::Camera(trikControl::BrickInterface &brick) : QQuickImageProvider(QQuickImageProvider::Image), mBrick(brick) {
}

QImage Camera::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
	Q_UNUSED(id)
	Q_UNUSED(size)
	Q_UNUSED(requestedSize)
	return mPhoto;
}

void Camera::renew()
{
	// Camera takes a single photo on construction. Periodic renew() is not needed.
}

void Camera::doPhoto()
{
	if (mIsCreatingPhoto.exchange(true)) {
		return;
	}

	auto const photo = trikControl::Utilities::rescalePhoto(mBrick.getStillImage());
	// imageFromBytes allocates memory and delete it when it is necessery
	auto image = trikControl::Utilities::imageFromBytes(photo, 160, 120, "rgb32");

	if (!image.isNull()) {
		mPhoto = image;
		Q_EMIT imageChanged();
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
		Q_EMIT cameraUnavailable();
	}

	mIsCreatingPhoto.store(false);
}

QString Camera::namePhoto() { return mNamePhoto; }
