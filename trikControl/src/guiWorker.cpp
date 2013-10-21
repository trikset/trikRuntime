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

#include "guiWorker.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QStackedLayout>
#else
	#include <QtWidgets/QStackedLayout>
	#include <QtWidgets/QPushButton>
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QDialog>
#endif

#include <QtCore/QThread>
#include <QtGui/QPixmap>

using namespace trikControl;

GuiWorker::GuiWorker()
{
	QHBoxLayout * const layout = new QHBoxLayout();
	mImageLabel.setScaledContents(true);
	layout->addWidget(&mImageLabel);
	mImageWidget.setLayout(layout);
	mImageWidget.setWindowState(Qt::WindowFullScreen);
}

void GuiWorker::showImage(QString const &fileName)
{
	QPixmap pixmap(fileName);
	pixmap = pixmap.scaled(mImageWidget.size() - QSize(20, 20), Qt::KeepAspectRatio);
	mImageLabel.setPixmap(pixmap);
	mImageWidget.show();
}

void GuiWorker::deleteWorker()
{
	deleteLater();
	thread()->quit();
}
