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

#include "lazyMainWidget.h"
#include "lazyMainWidgetWrapper.h"

using namespace trikGui;

BackgroundWidget::BackgroundWidget(const QString &configPath
		, const QString &startDirPath
		, QWidget *parent)
	: QWidget(parent)
	, mController(configPath, startDirPath)
	, mBatteryIndicator(mController.brick())
	, mStartWidget(mController, configPath)
	, mRunningWidget(mController)
{
	setWindowState(Qt::WindowFullScreen);

	mMainLayout.setSpacing(10);

	mBatteryIndicator.setStyleSheet("font: 12px");

	mStatusBarLayout.addWidget(&mBatteryIndicator);
	addMainWidget(mStartWidget);
	mBrickDisplayWidgetWrapper.reset(new LazyMainWidgetWrapper(&mController.brick().graphicsWidget()));
	addLazyWidget(*mBrickDisplayWidgetWrapper);
	mMainWidgetsLayout.addWidget(&mRunningWidget);

	mMainLayout.addLayout(&mStatusBarLayout);
	mMainLayout.addLayout(&mMainWidgetsLayout);

	setLayout(&mMainLayout);

	connect(&mMainWidgetsLayout, SIGNAL(currentChanged(int)), this, SLOT(renewFocus()));
	connect(&mMainWidgetsLayout, SIGNAL(widgetRemoved(int)), this, SLOT(updateStack(int)));

	connect(&mController, SIGNAL(brickStopped()), this, SLOT(refresh()));
	connect(&mController, SIGNAL(showRunningWidget(QString, int)), this, SLOT(showRunningWidget(QString, int)));
	connect(&mController, SIGNAL(showError(QString, int)), this, SLOT(showError(QString, int)));
	connect(&mController, SIGNAL(hideRunningWidget(int)), this, SLOT(hideRunningWidget(int)));
	connect(&mController, SIGNAL(hideGraphicsWidget()), this, SLOT(hideGraphicsWidget()));
	connect(&mController, SIGNAL(hideScriptWidgets()), this, SLOT(hideScriptWidgets()));

	connect(&mRunningWidget, SIGNAL(hideMe(int)), this, SLOT(hideRunningWidget(int)));
}

void BackgroundWidget::resetWidgetLayout(MainWidget &widget)
{
	// If the widget has layout, remove its margins because main widgets layout has its own margins.
	QLayout *layout = widget.layout();
	if (layout != nullptr) {
		layout->setContentsMargins(0, 0, 0, 0);
	}
}

void BackgroundWidget::addMainWidget(MainWidget &widget)
{
	resetWidgetLayout(widget);

	mMainWidgetIndex.push(mMainWidgetsLayout.addWidget(&widget));
	mMainWidgetsLayout.setCurrentIndex(mMainWidgetIndex.top());

	connect(&widget, SIGNAL(newWidget(MainWidget &)), this, SLOT(addMainWidget(MainWidget &)));
}

void BackgroundWidget::addRunningWidget(MainWidget &widget)
{
	resetWidgetLayout(widget);
	mMainWidgetsLayout.addWidget(&widget);
}

void BackgroundWidget::addLazyWidget(LazyMainWidget &widget)
{
	resetWidgetLayout(widget);
	mMainWidgetsLayout.addWidget(&widget);

	connect(&widget, SIGNAL(showMe(MainWidget &)), this, SLOT(showMainWidget(MainWidget &)));
	connect(&widget, SIGNAL(hideMe()), this, SLOT(hideGraphicsWidget()));
}

void BackgroundWidget::showMainWidget(MainWidget &widget)
{
	const int index = mMainWidgetsLayout.indexOf(&widget);
	if (index >= 0) {
		mMainWidgetsLayout.setCurrentIndex(index);
	}
}

void BackgroundWidget::showRunningWidget(const QString &fileName, int scriptId)
{
	mRunningWidget.setProgram(fileName, scriptId);
	mMainWidgetsLayout.setCurrentWidget(&mRunningWidget);
	mRunningWidget.grabKeyboard();
}

void BackgroundWidget::hideRunningWidget(int scriptId)
{
	if (mRunningWidget.scriptId() == scriptId) {
		mMainWidgetsLayout.setCurrentIndex(mMainWidgetIndex.top());
		mRunningWidget.releaseKeyboard();
	}
}

void BackgroundWidget::showError(const QString &error, int scriptId)
{
	if (mRunningWidget.scriptId() == scriptId) {
		mRunningWidget.showError(error, scriptId);
		mMainWidgetsLayout.setCurrentWidget(&mRunningWidget);
	}
}

void BackgroundWidget::hideGraphicsWidget()
{
	if (mMainWidgetsLayout.currentWidget() == &mController.brick().graphicsWidget()) {
		mMainWidgetsLayout.setCurrentWidget(&mRunningWidget);
	}
}

void BackgroundWidget::hideScriptWidgets()
{
	mMainWidgetsLayout.setCurrentIndex(mMainWidgetIndex.top());
	mRunningWidget.releaseKeyboard();
}

void BackgroundWidget::renewFocus()
{
	// When current widget in main widgets layout changed, we should set focus properly.

	MainWidget *currentWidget = dynamic_cast<MainWidget *>(mMainWidgetsLayout.currentWidget());

	if (currentWidget != nullptr) {
		currentWidget->renewFocus();
	}
}

void BackgroundWidget::refresh()
{
	for (const auto widget : QApplication::allWidgets()) {
		widget->update();
	}
}

void BackgroundWidget::updateStack(int removedWidget)
{
    Q_ASSERT(!mMainWidgetIndex.isEmpty() && "Closing main widget is prohibited");
    // or allowed ???
    // if (mMainWidgetIndex.isEmpty()) return;

	if (mMainWidgetIndex.top() == removedWidget) {
		mMainWidgetIndex.pop();
		mMainWidgetsLayout.setCurrentIndex(mMainWidgetIndex.top());
	}
}
