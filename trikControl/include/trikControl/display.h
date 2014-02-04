/* Copyright 2013 Yurii Litvinov
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

#include <QtCore/QThread>

#include "declSpec.h"

namespace trikControl {

class GuiWorker;

/// Provides ability to draw something on robot display.
class TRIKCONTROL_EXPORT Display : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param guiThread - GUI thread of an application.
	explicit Display(QThread &guiThread);

	~Display();

public slots:
	/// Shows given image on a display.
	/// @param fileName - file name (with path) of an image to show. Refer to Qt documentation for
	/// supported formats, but .jpg, .png, .bmp, .gif are supported.
	void showImage(QString const &fileName);

	/// Shortcut to showImage, shows happy smile.
	void smile();

	/// Shortcut to showImage, shows sad smile.
	void sadSmile();

	/// Sets background for a picture.
	/// @param color - color of a background.
	void setBackground(QString const &color);

	void hide();

signals:
	void threadShowImage(QString const &fileName);
	void threadSetBackground(QString const &color);
	void threadHide();
	void threadDelete();

private:
	QThread &mGuiThread;
	GuiWorker *mGuiWorker;  // Has ownership.
};

}
