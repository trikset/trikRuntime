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
}

void BackgroundWidget::addMainWidget(MainWidget &widget)
{
	// If the widget has layout, remove its margins because main widgets layout has its own margins.
	QLayout *layout = widget.layout();
	if (layout != nullptr) {
		layout->setContentsMargins(0, 0, 0, 0);
	}

	int const index = mMainWidgetsLayout.addWidget(&widget);
	mMainWidgetsLayout.setCurrentIndex(index);

	connect(&widget, SIGNAL(newWidget(MainWidget &)), this, SLOT(addMainWidget(MainWidget &)));
}

void BackgroundWidget::renewFocus()
{
	// When current widget in main widgets layout changed, we should set focus properly.

	MainWidget *currentWidget = dynamic_cast<MainWidget *>(mMainWidgetsLayout.currentWidget());

	if (currentWidget != nullptr) {
		currentWidget->renewFocus();
	}
}
