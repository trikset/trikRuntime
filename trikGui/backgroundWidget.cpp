/* Copyright 2014 CyberTech Labs Ltd.
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

#include "backgroundWidget.h"

#include <QtCore/QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

using namespace trikGui;

BackgroundWidget::BackgroundWidget(QString const &configPath
		, QString const &startDirPath
		, QWidget *parent)
	: QWidget(parent)
	, mController(configPath, startDirPath)
	, mBatteryIndicator(mController.brick())
	, mStartWidget(mController, configPath)
{
	setWindowState(Qt::WindowFullScreen);

	mMainLayout.setSpacing(10);

	mBatteryIndicator.setStyleSheet("font: 12px");

	mStatusBarLayout.addWidget(&mBatteryIndicator);
	addMainWidget(mStartWidget);

	mMainLayout.addLayout(&mStatusBarLayout);
	mMainLayout.addLayout(&mMainWidgetsLayout);

	setLayout(&mMainLayout);

	connect(&mMainWidgetsLayout, SIGNAL(currentChanged(int)), this, SLOT(renewFocus()));
	connect(&mController, SIGNAL(addRunningWidget(trikKernel::MainWidget &)), this, SLOT(addMainWidget(trikKernel::MainWidget &)));
	connect(&mController, SIGNAL(closeRunningWidget(trikKernel::MainWidget &)), this, SLOT(closeMainWidget(trikKernel::MainWidget &)));
	connect(&mController, SIGNAL(brickStopped()), this, SLOT(refresh()));
	connect(&mController, SIGNAL(addGraphicsWidget(trikKernel::LazyMainWidget &)), this, SLOT(addLazyWidget(trikKernel::LazyMainWidget &)));
	connect(&mController, SIGNAL(closeGraphicsWidget(trikKernel::MainWidget &)), this, SLOT(closeMainWidget(trikKernel::MainWidget &)));
}

void BackgroundWidget::addMainWidget(trikKernel::MainWidget &widget)
{
	qDebug() << "BackgroundWidget::addMainWidget";
	addMainWidgetWithoutConnect(widget);

	connect(&widget, SIGNAL(newWidget(trikKernel::MainWidget &)), this, SLOT(addMainWidget(trikKernel::MainWidget &)));
}

void BackgroundWidget::addMainWidgetWithoutConnect(trikKernel::MainWidget &widget)
{
	qDebug() << "BackgroundWidget::addMainWidgetWithoutConnect_01";

	// If the widget has layout, remove its margins because main widgets layout has its own margins.
	QLayout *layout = widget.layout();
	if (layout != nullptr) {
		layout->setContentsMargins(0, 0, 0, 0);
	}

//if (mMainWidgetsLayout.currentWidget() != *widget) {
		int const index = mMainWidgetsLayout.addWidget(&widget);
		mMainWidgetsLayout.setCurrentIndex(index);
	qDebug() << "BackgroundWidget::addMainWidgetWithoutConnect_02" << index;
	//}
}

void BackgroundWidget::closeMainWidget(trikKernel::MainWidget &widget)
{
	qDebug() << "BackgroundWidget::closeMainWidget";
	mMainWidgetsLayout.removeWidget(&widget);
}

void BackgroundWidget::abortLazyMainWidget(trikKernel::LazyMainWidget &widget)
{
	qDebug() << "BackgroundWidget::abortLazyMainWidget";
	mController.abortExecution();
	closeMainWidget(widget);
}

void BackgroundWidget::showMainWidget(trikKernel::MainWidget &widget)
{
	int const index = mMainWidgetsLayout.indexOf(&widget);
	mMainWidgetsLayout.setCurrentIndex(index);
}

void BackgroundWidget::hideMainWidget(trikKernel::MainWidget &widget)
{
	int const index = mMainWidgetsLayout.indexOf(&widget);
	int const currentIndex = mMainWidgetsLayout.currentIndex();
	qDebug() << "BackgroundWidget::hideMainWidget" << index << currentIndex;

	if (index == currentIndex && index > 0) {// else: this widget is hidden already
		mMainWidgetsLayout.setCurrentIndex(index - 1);
	}
}

void BackgroundWidget::addLazyWidget(trikKernel::LazyMainWidget &widget)
{
	qDebug() << "BackgroundWidget::addLazyWidget" << widget.toolTip();
	addMainWidgetWithoutConnect(widget);
	connect(&widget, SIGNAL(showMe(trikKernel::MainWidget &)), this, SLOT(showMainWidget(trikKernel::MainWidget &)));
	connect(&widget, SIGNAL(hideMe(trikKernel::MainWidget &)), this, SLOT(hideMainWidget(trikKernel::MainWidget &)));
	//connect(&widget, SIGNAL(abortMe(trikKernel::MainWidget &)), this, SLOT(closeMainWidget(trikKernel::MainWidget &)));
}

void BackgroundWidget::renewFocus()
{
	// When current widget in main widgets layout changed, we should set focus properly.

	trikKernel::MainWidget *currentWidget = dynamic_cast<trikKernel::MainWidget *>(mMainWidgetsLayout.currentWidget());

	if (currentWidget != nullptr) {
		currentWidget->renewFocus();
	}
}

void BackgroundWidget::refresh()
{
	for (auto const widget : QApplication::allWidgets()) {
		widget->update();
	}
}

