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

#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

#include <atomic>
#include "abstractIndicator.h"

namespace trikControl {
class BrickInterface;
}

namespace trikGui {

/// Widget that demonstrates availabilty of the camera and photo from it
class CameraWidget : public AbstractIndicator
{
	Q_OBJECT
public:
	/// Constructor for Camera Widget in testing menu
	explicit CameraWidget(trikControl::BrickInterface &brick, QWidget *parent = nullptr);

public slots:
	void renew() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	/// Creating image to display
	void doPhoto();

	QVBoxLayout mLayout;
	std::atomic_bool mIsCreatingPhoto {false};
	QLabel mTitle;
	QLabel mPixmap;
	trikControl::BrickInterface &mBrick;
};

}
