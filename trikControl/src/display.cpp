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

Display::Display(QThread &guiThread)
	: mGuiThread(guiThread)
{
	mGuiWorker = new GuiWorker();
	mGuiWorker->moveToThread(&guiThread);

	connect(this, SIGNAL(threadShowImage(QString)), mGuiWorker, SLOT(showImage(QString)));
	connect(this, SIGNAL(threadAddLabel(QString, int, int)), mGuiWorker, SLOT(addLabel(QString, int, int)));
	connect(this, SIGNAL(threadRemoveLabels()), mGuiWorker, SLOT(removeLabels()));
	connect(this, SIGNAL(threadSetBackground(QString)), mGuiWorker, SLOT(setBackground(QString)));
	connect(this, SIGNAL(threadHide()), mGuiWorker, SLOT(hide()));
	connect(this, SIGNAL(threadDelete()), mGuiWorker, SLOT(deleteWorker()));
	connect(this, SIGNAL(threadClear()), mGuiWorker, SLOT(clear()));
	connect(this, SIGNAL(threadDrawLine(int,int,int,int)), mGuiWorker, SLOT(drawLine(int,int,int,int)));
	connect(this, SIGNAL(threadDrawPoint(int,int)), mGuiWorker, SLOT(drawPoint(int,int)));
	connect(this, SIGNAL(threadDrawRect(int,int,int,int)), mGuiWorker, SLOT(drawRect(int,int,int,int)));
	connect(this, SIGNAL(threadDrawEllipse(int,int,int,int)), mGuiWorker, SLOT(drawEllipse(int,int,int,int)));
	connect(this, SIGNAL(threadDrawArc(int,int,int,int,int,int)), mGuiWorker, SLOT(drawArc(int,int,int,int,int,int)));
	connect(this, SIGNAL(threadSetPainterColor(QString)), mGuiWorker, SLOT(setPainterColor(QString)));
	connect(this, SIGNAL(threadSetPainterWidth(int)), mGuiWorker, SLOT(setPainterWidth(int)));
}

Display::~Display()
{
	emit threadDelete();
	mGuiThread.wait(1000);
}

void Display::showImage(QString const &fileName)
{
	emit threadShowImage(fileName);
}

void Display::addLabel(QString const &text, int x, int y)
{
	emit threadAddLabel(text, x, y);
}

void Display::removeLabels()
{
	emit threadRemoveLabels();
}

void Display::smile()
{
	showImage("media/trik_smile_normal.png");
}

void Display::sadSmile()
{
	showImage("media/trik_smile_sad.png");
}

void Display::setBackground(QString const &color)
{
	emit threadSetBackground(color);
}

void Display::hide()
{
	emit threadHide();
}

void Display::clear()
{
	emit threadClear();
}

void Display::drawLine(int x1, int y1, int x2, int y2)
{
	emit threadDrawLine(x1, y1, x2, y2);
}

void Display::drawPoint(int x, int y)
{
	emit threadDrawPoint(x, y);
}

void Display::drawRect(int x, int y, int width, int height)
{
	emit threadDrawRect(x, y, width, height);
}

void Display::drawEllipse(int x, int y, int width, int height)
{
	emit threadDrawEllipse(x, y, width, height);
}

void Display::drawArc(int x, int y, int width, int height, int startAngle, int spanAngle)
{
	emit threadDrawArc(x, y, width, height, startAngle, spanAngle);
}

void Display::setPainterColor(QString const &color)
{
	emit threadSetPainterColor(color);
}

void Display::setPainterWidth(int penWidth)
{
	emit threadSetPainterWidth(penWidth);
}
