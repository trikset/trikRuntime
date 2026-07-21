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

#pragma once

#include "abstractIndicator.h"
#include <QQuickImageProvider>
#include <atomic>

namespace trikControl {
class BrickInterface;
}

namespace trikGui {

/// Widget that demonstrates availabilty of the camera and photo from it
class Camera : public AbstractIndicator, public QQuickImageProvider
{
	Q_OBJECT
	Q_PROPERTY(QString namePhoto READ namePhoto NOTIFY namePhotoChanged)
public:
	/// Constructor for Camera Widget in testing menu
	explicit Camera(trikControl::BrickInterface &brick);

	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

public Q_SLOTS:
	void renew() override;
	/// Takes a new photo and emits imageChanged() or cameraUnavailable()
	Q_INVOKABLE void doPhoto();

private:
	QString namePhoto();
	int count{0};

	std::atomic_bool mIsCreatingPhoto {false};
	trikControl::BrickInterface &mBrick;
	QString mNamePhoto;
	QImage mPhoto;

Q_SIGNALS:
	/// Emitted when name photo changed
	void namePhotoChanged();
	/// Emitted when image changed
	void imageChanged();
	/// Emitted when camera unavailable
	void cameraUnavailable();
};

}
