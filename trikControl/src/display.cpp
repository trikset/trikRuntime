/* Copyright 2013 - 2015 CyberTech Labs Ltd, Smirnov Mikhail, Kogutich Denis
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

#include "display.h"

#include <QtCore/QFileInfo>
#include <qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include <QsLog.h>

#include "src/guiWorker.h"

trikControl::Display::Display(const QString &mediaPath)
	: mMediaPath(mediaPath)
	, mGuiWorker(new GuiWorker())
{
	mGuiWorker->setParent(this);
	if (!qApp) {
		QLOG_ERROR() << "No QApplication object, it seems that trikControl is used from console application";
		return;
	}

	mGuiWorker->moveToThread(qApp->thread());
	QMetaObject::invokeMethod(mGuiWorker, [this](){mGuiWorker->init();});
}

trikControl::Display::~Display()
{
	mGuiWorker->deleteLater();
}

trikControl::DisplayWidgetInterface &trikControl::Display::graphicsWidget()
{
	return mGuiWorker->graphicsWidget();
}

void trikControl::Display::showImage(const QString &fileName)
{
	QFileInfo imageFile(fileName);
	const QString correctedFileName = imageFile.exists() ? fileName : mMediaPath + fileName;
	QMetaObject::invokeMethod(mGuiWorker,
				  [this, correctedFileName](){mGuiWorker->showImage(correctedFileName);});
}

void trikControl::Display::show(const QVector<int32_t> &array, int width, int height, const QString &format)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, array, width, height, format](
				  ){mGuiWorker->show(array, width, height, format);});
}

void trikControl::Display::addLabel(const QString &text, int x, int y)
{
	QMetaObject::invokeMethod(mGuiWorker,[this, text, x, y](){mGuiWorker->addLabel(text, x, y);});
}

void trikControl::Display::removeLabels()
{
	QMetaObject::invokeMethod(mGuiWorker, [this](){mGuiWorker->removeLabels();});
}

void trikControl::Display::setBackground(const QString &color)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, color](){mGuiWorker->setBackground(color);});
}

void trikControl::Display::hide()
{
	QMetaObject::invokeMethod(mGuiWorker, [this](){mGuiWorker->hide();});
}

void trikControl::Display::clear()
{
	QMetaObject::invokeMethod(mGuiWorker, [this](){mGuiWorker->clear();});
}

void trikControl::Display::reset()
{
	QMetaObject::invokeMethod(mGuiWorker, [this](){mGuiWorker->reset();});
}

void trikControl::Display::redraw()
{
	QMetaObject::invokeMethod(mGuiWorker, [this](){mGuiWorker->redraw();});
}

void trikControl::Display::drawLine(int x1, int y1, int x2, int y2)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, x1, y1, x2, y2](){mGuiWorker->drawLine(x1, y1, x2, y2);});
}

void trikControl::Display::drawPoint(int x, int y)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, x, y](){mGuiWorker->drawPoint(x, y);});
}

void trikControl::Display::drawRect(int x, int y, int width, int height, bool filled)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, x, y, width, height, filled](
				  ){mGuiWorker->drawRect(x, y, width, height, filled);});
}

void trikControl::Display::drawEllipse(int x, int y, int width, int height, bool filled)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, x, y, width, height, filled](
				  ){mGuiWorker->drawEllipse(x, y, width, height, filled);});
}

void trikControl::Display::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, x, y, width, height, startAngle, spanAngle](
				  ){mGuiWorker->drawArc(x, y, width, height, startAngle, spanAngle);});
}

void trikControl::Display::setPainterColor(const QString &color)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, color](){mGuiWorker->setPainterColor(color);});
}

void trikControl::Display::setPainterWidth(int penWidth)
{
	QMetaObject::invokeMethod(mGuiWorker, [this, penWidth](){mGuiWorker->setPainterWidth(penWidth);});
}
