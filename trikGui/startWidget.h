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
#include <QtCore/QMap>

#include "wiFiModeWidget.h"

#include "controller.h"

namespace trikGui {

/// Main widget for trikGui application, displays menu which allows to open other widgets.
class StartWidget : public QWidget
{
	Q_OBJECT

public:
	/// Constructor
	/// @param configPath - full path to configuration files.
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit StartWidget(QString const &configPath, QWidget *parent = 0);

	~StartWidget();

private:
	virtual void keyPressEvent(QKeyEvent *event);

	void launch();

	void setRootIndex(QModelIndex const &index);

	void goHome();

	QVBoxLayout mLayout;
	QLabel mTitleLabel;
	QListView mMenuView;
	QStandardItemModel mMenuModel;
	Controller mController;
	QString const mConfigPath;

	/// Stores index of item which was selected in a submenu after a last visit.
	QMap<QPersistentModelIndex, QPersistentModelIndex> mSelections;
};

}
