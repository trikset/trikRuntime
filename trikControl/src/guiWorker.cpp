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

#include <QtCore/QDebug>

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
	mImageWidget.setWindowFlags(mImageWidget.windowFlags() | Qt::WindowStaysOnTopHint);
	resetBackground();
}

void GuiWorker::showImage(QString const &fileName)
{
	if (!mImagesCache.contains(fileName)) {
		QPixmap pixmap(fileName);
		pixmap = pixmap.scaled(mImageWidget.size() - QSize(20, 20), Qt::KeepAspectRatio);
		mImagesCache.insert(fileName, pixmap);
	}

	mImageLabel.setPixmap(mImagesCache.value(fileName));
	mImageWidget.show();
}

void GuiWorker::addLabel(QString const &text, int x, int y)
{
	QLabel *label = findLabel(x, y);
	label = label ? label : new QLabel(&mImageWidget);
	label->setText(text);
	label->setGeometry(x, y, label->width(), label->height());
	label->show();
	if (!mLabels.contains(x ^ y, label)) {
		mLabels.insertMulti(x ^ y, label);
	}

	mImageWidget.show();
}

void GuiWorker::removeLabels()
{
	foreach (QLabel * const label, mLabels.values()) {
		label->close();
		delete label;
	}

	mLabels.clear();
}

void GuiWorker::deleteWorker()
{
	deleteLater();
}

void GuiWorker::setBackground(QString const &color)
{
	QPalette palette = mImageWidget.palette();

	if (color == tr("white")) {
		palette.setColor(QPalette::Window, Qt::white);
	} else if (color == tr("black")) {
		palette.setColor(QPalette::Window, Qt::black);
	} else if (color == tr("red")) {
		palette.setColor(QPalette::Window, Qt::red);
	} else if (color == tr("darkRed")) {
		palette.setColor(QPalette::Window, Qt::darkRed);
	} else if (color == tr("green")) {
		palette.setColor(QPalette::Window, Qt::green);
	} else if (color == tr("darkGreen")) {
		palette.setColor(QPalette::Window, Qt::darkGreen);
	} else if (color == tr("blue")) {
		palette.setColor(QPalette::Window, Qt::blue);
	} else if (color == tr("darkBlue")) {
		palette.setColor(QPalette::Window, Qt::darkBlue);
	} else if (color == tr("cyan")) {
		palette.setColor(QPalette::Window, Qt::cyan);
	} else if (color == tr("darkCyan")) {
		palette.setColor(QPalette::Window, Qt::darkCyan);
	} else if (color == tr("magenta")) {
		palette.setColor(QPalette::Window, Qt::magenta);
	} else if (color == tr("darkMagenta")) {
		palette.setColor(QPalette::Window, Qt::darkMagenta);
	} else if (color == tr("yellow")) {
		palette.setColor(QPalette::Window, Qt::yellow);
	} else if (color == tr("darkYellow")) {
		palette.setColor(QPalette::Window, Qt::darkYellow);
	} else if (color == tr("gray")) {
		palette.setColor(QPalette::Window, Qt::gray);
	} else if (color == tr("darkGray")) {
		palette.setColor(QPalette::Window, Qt::darkGray);
	} else if (color == tr("lightGray")) {
		palette.setColor(QPalette::Window, Qt::lightGray);
	} else {
		palette.setColor(QPalette::Window, QColor(color));
	}

	mImageWidget.setPalette(palette);
	mImageWidget.show();
}

void GuiWorker::resetBackground()
{
	QPalette palette = mImageWidget.palette();
	palette.setColor(QPalette::Window, Qt::lightGray);
	mImageWidget.setPalette(palette);
}

void GuiWorker::clear()
{
	mImageWidget.hide();
	removeLabels();
	mImageLabel.setPixmap(QPixmap());
	resetBackground();
}

void GuiWorker::hide()
{
	mImageWidget.hide();
}

QLabel *GuiWorker::findLabel(int x, int y) const
{
	foreach (QLabel * const label, mLabels.values(x ^ y)) {
		if (label->x() == x && label->y() == y) {
			return label;
		}
	}

	return NULL;
}
