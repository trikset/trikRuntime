/* Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#include "startWidget.h"

#include <QtGui/QKeyEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include "fileManagerWidget.h"
#include "netConfigWidget.h"

using namespace trikGui;

StartWidget::StartWidget(QString const &configPath, QWidget *parent)
	: QWidget(parent)
	, mController(configPath)
	, mConfigPath(configPath)
{
	mExitItemTitle = tr("Exit");

	setWindowState(Qt::WindowFullScreen);

	mTitleLabel.setText(tr("TRIK"));

	mMenuItems.append(new QStandardItem(FileManagerWidget::menuEntry()));
	mMenuItems.append(new QStandardItem(NetConfigWidget::menuEntry()));
	mMenuItems.append(new QStandardItem(mExitItemTitle));

	mMenuModel.appendColumn(mMenuItems);

	mMenuView.setModel(&mMenuModel);

	mLayout.addWidget(&mTitleLabel);
	mLayout.addWidget(&mMenuView);

	setLayout(&mLayout);
}

StartWidget::~StartWidget()
{
}

void StartWidget::launch()
{
	QString const &currentItemText = mMenuModel.itemFromIndex(mMenuView.currentIndex())->text();
	if (currentItemText == FileManagerWidget::menuEntry()) {
		FileManagerWidget *fileManagerWidget = new FileManagerWidget(mController);
		fileManagerWidget->show();
	} else if (currentItemText == NetConfigWidget::menuEntry()) {
		NetConfigWidget *netConfigWidget = new NetConfigWidget(mConfigPath);
		netConfigWidget->show();
	} else if (currentItemText == mExitItemTitle) {
		QApplication::exit(0);
	}
}

void StartWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Meta: {
			break;
		}
		case Qt::Key_Enter: {
			launch();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
