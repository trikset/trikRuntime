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
	if (!qApp) {
		QLOG_ERROR() << "No QApplication object, it seems that trikControl is used from console application";
		return;
	}

	mGuiWorker->moveToThread(qApp->thread());
	QMetaObject::invokeMethod(mGuiWorker, "init");
}

trikControl::Display::~Display()
{
	QMetaObject::invokeMethod(mGuiWorker, "deleteWorker");
	qApp->thread()->wait(1000);
}

trikControl::DisplayWidgetInterface &trikControl::Display::graphicsWidget()
{
	return mGuiWorker->graphicsWidget();
}

void trikControl::Display::showImage(const QString &fileName)
{
	QFileInfo imageFile(fileName);
	const QString correctedFileName = imageFile.exists() ? fileName : mMediaPath + fileName;
	QMetaObject::invokeMethod(mGuiWorker, "showImage", Q_ARG(QString, correctedFileName));
}

void trikControl::Display::addLabel(const QString &text, int x, int y)
{
	QMetaObject::invokeMethod(mGuiWorker, "addLabel", Q_ARG(QString, text), Q_ARG(int, x), Q_ARG(int, y));
}

void trikControl::Display::removeLabels()
{
	QMetaObject::invokeMethod(mGuiWorker, "removeLabels");
}

void trikControl::Display::setBackground(const QString &color)
{
	QMetaObject::invokeMethod(mGuiWorker, "setBackground", Q_ARG(QString, color));
}

void trikControl::Display::hide()
{
	QMetaObject::invokeMethod(mGuiWorker, "hide");
}

void trikControl::Display::clear()
{
	QMetaObject::invokeMethod(mGuiWorker, "clear");
}

void trikControl::Display::reset()
{
	QMetaObject::invokeMethod(mGuiWorker, "reset");
}

void trikControl::Display::redraw()
{
	QMetaObject::invokeMethod(mGuiWorker, "redraw");
}

void trikControl::Display::drawLine(int x1, int y1, int x2, int y2)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawLine", Q_ARG(int, x1), Q_ARG(int, y1), Q_ARG(int, x2), Q_ARG(int, y2));
}

void trikControl::Display::drawPoint(int x, int y)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawPoint", Q_ARG(int, x), Q_ARG(int, y));
}

void trikControl::Display::drawRect(int x, int y, int width, int height, bool filled)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawRect", Q_ARG(int, x), Q_ARG(int, y)
			, Q_ARG(int, width), Q_ARG(int, height), Q_ARG(bool, filled));
}

void trikControl::Display::drawEllipse(int x, int y, int width, int height, bool filled)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawEllipse", Q_ARG(int, x), Q_ARG(int, y)
			, Q_ARG(int, width), Q_ARG(int, height), Q_ARG(bool, filled));
}

void trikControl::Display::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawArc", Q_ARG(int, x), Q_ARG(int, y)
			, Q_ARG(int, width), Q_ARG(int, height), Q_ARG(int, startAngle), Q_ARG(int, spanAngle));
}

void trikControl::Display::setPainterColor(const QString &color)
{
	QMetaObject::invokeMethod(mGuiWorker, "setPainterColor", Q_ARG(QString, color));
}

void trikControl::Display::setPainterWidth(int penWidth)
{
	QMetaObject::invokeMethod(mGuiWorker, "setPainterWidth", Q_ARG(int, penWidth));
}
