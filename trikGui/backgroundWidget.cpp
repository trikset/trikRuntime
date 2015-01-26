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
	, mRunWidgetIndex(-1)
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
	connect(&mController, SIGNAL(addRunningWidget(trikKernel::MainWidget &))
			, this, SLOT(addRunningWidget(trikKernel::MainWidget &)));
	connect(&mController, SIGNAL(closeRunningWidget(trikKernel::MainWidget &))
			, this, SLOT(closeMainWidget(trikKernel::MainWidget &)));
	connect(&mController, SIGNAL(brickStopped()), this, SLOT(refresh()));
	connect(&mController, SIGNAL(addGraphicsWidget(trikKernel::LazyMainWidget &))
			, this, SLOT(addLazyWidget(trikKernel::LazyMainWidget &)));
	connect(&mController, SIGNAL(closeGraphicsWidget(trikKernel::MainWidget &))
			, this, SLOT(closeMainWidget(trikKernel::MainWidget &)));
}

void BackgroundWidget::resetWidgetLayout(trikKernel::MainWidget &widget)
{
	// If the widget has layout, remove its margins because main widgets layout has its own margins.
	QLayout *layout = widget.layout();
	if (layout != nullptr) {
		layout->setContentsMargins(0, 0, 0, 0);
	}
}

void BackgroundWidget::addMainWidget(trikKernel::MainWidget &widget)
{
	resetWidgetLayout(widget);

	int const index = mMainWidgetsLayout.addWidget(&widget);
	mMainWidgetsLayout.setCurrentIndex(index);

	connect(&widget, SIGNAL(newWidget(trikKernel::MainWidget &)), this, SLOT(addMainWidget(trikKernel::MainWidget &)));
}

void BackgroundWidget::addRunningWidget(trikKernel::MainWidget &widget)
{
	resetWidgetLayout(widget);

	mRunWidgetIndex = mMainWidgetsLayout.addWidget(&widget);
	mMainWidgetsLayout.setCurrentIndex(mRunWidgetIndex);
}

void BackgroundWidget::addLazyWidget(trikKernel::LazyMainWidget &widget)
{
	resetWidgetLayout(widget);

	mMainWidgetsLayout.addWidget(&widget);
	if (mRunWidgetIndex >= 0) {
		mMainWidgetsLayout.setCurrentIndex(mRunWidgetIndex);
	}

	connect(&widget, SIGNAL(showMe(trikKernel::MainWidget &)), this, SLOT(showMainWidget(trikKernel::MainWidget &)));
	connect(&widget, SIGNAL(hideMe()), this, SLOT(showRunningWidget()));
}

void BackgroundWidget::showMainWidget(trikKernel::MainWidget &widget)
{
	int const index = mMainWidgetsLayout.indexOf(&widget);
	if (index >= 0) {
		mMainWidgetsLayout.setCurrentIndex(index);
	}
}

void BackgroundWidget::showRunningWidget()
{
	if (mRunWidgetIndex >= 0) {
		mMainWidgetsLayout.setCurrentIndex(mRunWidgetIndex);
	}
}

void BackgroundWidget::closeMainWidget(trikKernel::MainWidget &widget)
{
	if (mMainWidgetsLayout.indexOf(&widget) == mRunWidgetIndex) {
		mRunWidgetIndex = -1;
	}

	mMainWidgetsLayout.removeWidget(&widget);
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

