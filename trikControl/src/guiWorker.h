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

#include <QtCore/qglobal.h>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtGui/QPixmap>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
#endif

namespace trikControl {

class GuiWorker : public QObject
{
	Q_OBJECT

public:
	GuiWorker();

public slots:
	void showImage(QString const &fileName);
	void addLabel(QString const &text, int x, int y);
	void removeLabels();
	void deleteWorker();
	void hide();

	/// Sets background for a picture.
	/// @param color - color of a background.
	void setBackground(QString const &color);

private:
	QWidget mImageWidget;
	QLabel mImageLabel;
	QHash<QString, QPixmap> mImagesCache;
	QList<QLabel *> mLabels;
};

}
