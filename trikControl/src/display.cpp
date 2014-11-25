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

#include "display.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QStackedLayout>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QStackedLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QPushButton>
#endif

#include "src/guiWorker.h"

using namespace trikControl;

Display::Display(QThread &guiThread, const QString &startDirPath)
	: mGuiThread(guiThread)
	, mStartDirPath(startDirPath)
	, mGuiWorker(new GuiWorker())
{
	mGuiWorker->moveToThread(&guiThread);
	QMetaObject::invokeMethod(mGuiWorker, "init");
}

Display::~Display()
{
	QMetaObject::invokeMethod(mGuiWorker, "deleteWorker");
	mGuiThread.wait(1000);
}

void Display::showImage(QString const &fileName)
{
	QMetaObject::invokeMethod(mGuiWorker, "showImage", Q_ARG(QString, mStartDirPath + fileName));
}

void Display::addLabel(QString const &text, int x, int y)
{
	QMetaObject::invokeMethod(mGuiWorker, "addLabel", Q_ARG(QString, text), Q_ARG(int, x), Q_ARG(int, y));
}

void Display::removeLabels()
{
	QMetaObject::invokeMethod(mGuiWorker, "removeLabels");
}

void Display::setBackground(QString const &color)
{
	QMetaObject::invokeMethod(mGuiWorker, "setBackground", Q_ARG(QString, color));
}

void Display::hide()
{
	QMetaObject::invokeMethod(mGuiWorker, "hide");
}

void Display::clear()
{
	QMetaObject::invokeMethod(mGuiWorker, "clear");
}

void Display::drawLine(int x1, int y1, int x2, int y2)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawLine", Q_ARG(int, x1), Q_ARG(int, y1), Q_ARG(int, x2), Q_ARG(int, y2));
}

void Display::drawPoint(int x, int y)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawPoint", Q_ARG(int, x), Q_ARG(int, y));
}

void Display::drawRect(int x, int y, int width, int height)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawRect", Q_ARG(int, x), Q_ARG(int, y)
			, Q_ARG(int, width), Q_ARG(int, height));
}

void Display::drawEllipse(int x, int y, int width, int height)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawEllipse", Q_ARG(int, x), Q_ARG(int, y)
			, Q_ARG(int, width), Q_ARG(int, height));
}

void Display::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	QMetaObject::invokeMethod(mGuiWorker, "drawArc", Q_ARG(int, x), Q_ARG(int, y)
			, Q_ARG(int, width), Q_ARG(int, height), Q_ARG(int, startAngle), Q_ARG(int, spanAngle));
}

void Display::setPainterColor(QString const &color)
{
	QMetaObject::invokeMethod(mGuiWorker, "setPainterColor", Q_ARG(QString, color));
}

void Display::setPainterWidth(int penWidth)
{
	QMetaObject::invokeMethod(mGuiWorker, "setPainterWidth", Q_ARG(int, penWidth));
}
