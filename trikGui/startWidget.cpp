/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "startWidget.h"

#include <QtGui/QKeyEvent>

#include "fileManagerWidget.h"
#include "netConfigWidget.h"

using namespace trikGui;

StartWidget::StartWidget(QWidget *parent)
	: QWidget(parent)
{
	mExitItemTitle = "Exit";

	setWindowState(Qt::WindowFullScreen);

	mTitleLabel.setText("TRIK");

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
		NetConfigWidget *netConfigWidget = new NetConfigWidget();
		netConfigWidget->show();
	} else if (currentItemText == mExitItemTitle) {
		close();
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
