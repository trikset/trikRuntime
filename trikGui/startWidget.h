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

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
	#include <QtGui/QListView>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QListView>
#endif

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
