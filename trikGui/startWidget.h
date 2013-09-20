/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtCore/QList>
#include <QtCore/QString>

#include "controller.h"

namespace trikGui {

class StartWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StartWidget(QWidget *parent = 0);
	~StartWidget();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	void launch();

	QString mExitItemTitle;
	QVBoxLayout mLayout;
	QLabel mTitleLabel;
	QListView mMenuView;
	QStandardItemModel mMenuModel;
	QList<QStandardItem *> mMenuItems;
	Controller mController;
};

}
