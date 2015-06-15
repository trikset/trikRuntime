/* Copyright 2013 - 2014 CyberTech Labs Ltd, Smirnov Mikhail, Kogutich Denis
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
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QStackedLayout>
	#include <QtWidgets/QPushButton>
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QDialog>
#endif

#include <QtGui/QPixmap>

using namespace trikControl;

GuiWorker::GuiWorker()
{
}

void GuiWorker::init()
{
	mImageWidget.reset(new GraphicsWidget());
	mImageWidget->setWindowState(Qt::WindowFullScreen);
	mImageWidget->setWindowFlags(mImageWidget->windowFlags() | Qt::WindowStaysOnTopHint);
	resetBackground();
}

DisplayWidgetInterface &GuiWorker::graphicsWidget()
{
	return *mImageWidget.data();
}

void GuiWorker::showImage(const QString &fileName)
{
	if (!mImagesCache.contains(fileName)) {
		QPixmap pixmap(fileName);
		pixmap = pixmap.scaled(mImageWidget->size() - QSize(20, 20), Qt::KeepAspectRatio);
		mImagesCache.insert(fileName, pixmap);
	}

	mImageWidget->setPixmap(mImagesCache.value(fileName));
	mImageWidget->update();
	mImageWidget->showCommand();
}

void GuiWorker::addLabel(const QString &text, int x, int y)
{
	mImageWidget->addLabel(text, x, y);
	mImageWidget->update();
	mImageWidget->showCommand();
}

void GuiWorker::removeLabels()
{
	mImageWidget->deleteLabels();
	mImageWidget->update();
	mImageWidget->showCommand();
}

void GuiWorker::deleteWorker()
{
	deleteLater();
}

void GuiWorker::setBackground(const QString &color)
{
	QPalette palette = mImageWidget->palette();

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

	mImageWidget->setPalette(palette);
	mImageWidget->showCommand();
}

void GuiWorker::resetBackground()
{
	QPalette palette = mImageWidget->palette();
	palette.setColor(QPalette::Window, Qt::transparent);
	mImageWidget->setPalette(palette);
}

void GuiWorker::setPainterColor(const QString &color)
{
	mImageWidget->setPainterColor(color);
}

void GuiWorker::setPainterWidth(int penWidth)
{
	mImageWidget->setPainterWidth(penWidth);
}

void GuiWorker::clear()
{
	QApplication::processEvents();  // process pending draw events so that they don't show after clearing
	mImageWidget->deleteAllItems();
	mImageWidget->setPainterColor("black");
	mImageWidget->setPainterWidth(1);
	mImageWidget->hideCommand();
	resetBackground();
}

void GuiWorker::hide()
{
	mImageWidget->hideCommand();
}

void GuiWorker::drawPoint(int x, int y)
{
	mImageWidget->drawPoint(x, y);
	mImageWidget->update();
	mImageWidget->showCommand();
}

void GuiWorker::drawLine(int x1, int y1, int x2, int y2)
{
	mImageWidget->drawLine(x1, y1, x2, y2);
	mImageWidget->update();
	mImageWidget->showCommand();
}

void GuiWorker::drawRect(int x, int y, int width, int height)
{
	mImageWidget->drawRect(x, y, width, height);
	mImageWidget->update();
	mImageWidget->showCommand();
}

void GuiWorker::drawEllipse(int x, int y, int width, int height)
{
	mImageWidget->drawEllipse(x, y, width, height);
	mImageWidget->update();
	mImageWidget->showCommand();
}

void GuiWorker::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	mImageWidget->drawArc(x, y, width, height, startAngle, spanAngle);
	mImageWidget->update();
	mImageWidget->showCommand();
}
