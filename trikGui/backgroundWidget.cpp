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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include "lazyMainWidget.h"
#include "lazyMainWidgetWrapper.h"
#include <QScreen>

using namespace trikGui;

BackgroundWidget::BackgroundWidget(
		const QString &configPath
		, QWidget *parent)
	: QWidget(parent)
	, mController(configPath)
	, mBatteryIndicator(mController.brick())
	, mWiFiIndicator(mController)
	, mGamepadIndicator(mController)
	, mMailboxIndicator("://resources/mailboxConnected.png", mController.mailbox()->isConnected())
	, mCommunicatorIndicator("://resources/communicatorConnected.png", mController.communicatorConnectionStatus())
	, mStartWidget(mController)
	, mRunningWidget(mController)
{
	setFixedSize(qApp->primaryScreen()->availableSize());
	mMainLayout.setSpacing(10);

	mBatteryIndicator.setStyleSheet("font: 12px");

	mStatusBarLayout.addWidget(&mBatteryIndicator);
	mStatusBarLayout.addStretch();
	mStatusBarLayout.addWidget(&mGamepadIndicator);
	mStatusBarLayout.addWidget(&mMailboxIndicator);
	mStatusBarLayout.addWidget(&mCommunicatorIndicator);
	mStatusBarLayout.addWidget(&mWiFiIndicator);
	addMainWidget(mStartWidget);
	mBrickDisplayWidgetWrapper.reset(new LazyMainWidgetWrapper(mController.brick().graphicsWidget()));
	addLazyWidget(*mBrickDisplayWidgetWrapper);
	mMainWidgetsLayout.addWidget(&mRunningWidget);

	mMainLayout.setContentsMargins(mDefaultMargins);
	mMainLayout.addLayout(&mStatusBarLayout);
	mMainLayout.addLayout(&mMainWidgetsLayout);

	setLayout(&mMainLayout);

	connect(&mMainWidgetsLayout, &QStackedLayout::currentChanged, this, &BackgroundWidget::renewFocus);
	connect(&mMainWidgetsLayout, &QStackedLayout::widgetRemoved, this, &BackgroundWidget::updateStack);

	connect(&mController, &Controller::brickStopped, this, &BackgroundWidget::refresh);
	connect(&mController, &Controller::showRunningWidget, this, &BackgroundWidget::showRunningWidget);
	connect(&mController, &Controller::showError, this, &BackgroundWidget::showError);
	connect(&mController, &Controller::hideRunningWidget, this, &BackgroundWidget::hideRunningWidget);
	connect(&mController, &Controller::hideGraphicsWidget, this, &BackgroundWidget::hideGraphicsWidget);
	connect(&mController, &Controller::hideScriptWidgets, this, &BackgroundWidget::hideScriptWidgets);
	connect(&mController, &Controller::communicatorStatusChanged,
			&mCommunicatorIndicator, &OpenSocketIndicator::changeStatus);
	connect(&mController, &Controller::mailboxStatusChanged, &mMailboxIndicator, &OpenSocketIndicator::changeStatus);

	connect(&mRunningWidget, &RunningWidget::hideMe, this, &BackgroundWidget::hideRunningWidget);

	mController.brick().playTone(800, 15);
}

BackgroundWidget::~BackgroundWidget()
{
	// Disconnect is needed here because QWidget destructor will trigger widgetRemoved signal which will be caught
	// here by partially deleted object and everything will crash.
	disconnect(&mMainWidgetsLayout, 0, 0, 0);
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

	connect(&widget, &MainWidget::newWidget, this, &BackgroundWidget::addMainWidget);
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

	connect(&widget, &LazyMainWidget::showMe, this, &BackgroundWidget::showMainWidget);
	connect(&widget, &LazyMainWidget::hideMe, this, &BackgroundWidget::hideGraphicsWidget);
}

void BackgroundWidget::showMainWidget(MainWidget &widget)
{
	if (&widget == mBrickDisplayWidgetWrapper.data()) {
		expandMainWidget();
	}

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
	if (mMainWidgetsLayout.currentWidget() == mBrickDisplayWidgetWrapper.data()) {
		unexpandMainWidget();
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
	if (mMainWidgetIndex.top() == removedWidget) {
		mMainWidgetIndex.pop();
		mMainWidgetsLayout.setCurrentIndex(mMainWidgetIndex.top());
	}
}

void BackgroundWidget::expandMainWidget()
{
	mMainLayout.setContentsMargins(0, 0, 0, 0);
	QMargins margins = mDefaultMargins;
	margins.setBottom(0);
	mStatusBarLayout.setContentsMargins(margins);
}

void BackgroundWidget::unexpandMainWidget()
{
	mMainLayout.setContentsMargins(mDefaultMargins);
	mStatusBarLayout.setContentsMargins(0, 0, 0, 0);
}
