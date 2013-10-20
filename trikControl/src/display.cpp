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
	connect(this, SIGNAL(threadDelete()), mGuiWorker, SLOT(deleteWorker()));
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

void Display::smile()
{
	showImage("media/trik_smile_normal.png");
}

void Display::sadSmile()
{
	showImage("media/trik_smile_sad.png");
}
