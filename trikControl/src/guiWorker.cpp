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

	mImageWidget->setPixmap(mImagesCache[fileName]);
	repaintGraphicsWidget();
}

void GuiWorker::addLabel(const QString &text, int x, int y)
{
	mImageWidget->addLabel(text, x, y);
}

void GuiWorker::removeLabels()
{
	mImageWidget->deleteLabels();
	repaintGraphicsWidget();
}

void GuiWorker::deleteWorker()
{
	deleteLater();
}

void GuiWorker::setBackground(const QString &color)
{
	QPalette palette = mImageWidget->palette();
	palette.setColor(QPalette::Window, colorByName(color));
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
	mImageWidget->setPainterColor(colorByName(color));
}

void GuiWorker::setPainterWidth(int penWidth)
{
	mImageWidget->setPainterWidth(penWidth);
}

void GuiWorker::clear()
{
	mImageWidget->deleteAllItems();
	mImageWidget->setPainterColor("black");
	mImageWidget->setPainterWidth(1);
	mImageWidget->hideCommand();
	resetBackground();
}

void GuiWorker::reset()
{
	QApplication::removePostedEvents(this, QEvent::MetaCall);
	QApplication::removePostedEvents(mImageWidget.data(), QEvent::Paint);
	clear();
}

void GuiWorker::hide()
{
	mImageWidget->hideCommand();
}

void GuiWorker::drawPoint(int x, int y)
{
	mImageWidget->drawPoint(x, y);
}

void GuiWorker::drawLine(int x1, int y1, int x2, int y2)
{
	mImageWidget->drawLine(x1, y1, x2, y2);
}

void GuiWorker::drawRect(int x, int y, int width, int height, bool filled)
{
	mImageWidget->drawRect(x, y, width, height, filled);
}

void GuiWorker::drawEllipse(int x, int y, int width, int height, bool filled)
{
	mImageWidget->drawEllipse(x, y, width, height, filled);
}

void GuiWorker::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	mImageWidget->drawArc(x, y, width, height, startAngle * 16, spanAngle * 16);
}

void GuiWorker::redraw()
{
	repaintGraphicsWidget();
}

void GuiWorker::repaintGraphicsWidget()
{
	mImageWidget->update();
	mImageWidget->showCommand();
}

QColor GuiWorker::colorByName(const QString &name)
{
	if (name == tr("white")) {
		return Qt::white;
	} else if (name == tr("black")) {
		return Qt::black;
	} else if (name == tr("red")) {
		return Qt::red;
	} else if (name == tr("darkRed")) {
		return Qt::darkRed;
	} else if (name == tr("green")) {
		return Qt::green;
	} else if (name == tr("darkGreen")) {
		return Qt::darkGreen;
	} else if (name == tr("blue")) {
		return Qt::blue;
	} else if (name == tr("darkBlue")) {
		return Qt::darkBlue;
	} else if (name == tr("cyan")) {
		return Qt::cyan;
	} else if (name == tr("darkCyan")) {
		return Qt::darkCyan;
	} else if (name == tr("magenta")) {
		return Qt::magenta;
	} else if (name == tr("darkMagenta")) {
		return Qt::darkMagenta;
	} else if (name == tr("yellow")) {
		return Qt::yellow;
	} else if (name == tr("darkYellow")) {
		return Qt::darkYellow;
	} else if (name == tr("gray")) {
		return Qt::gray;
	} else if (name == tr("darkGray")) {
		return Qt::darkGray;
	} else if (name == tr("lightGray")) {
		return Qt::lightGray;
	} else {
		return QColor(name);
	}
}
