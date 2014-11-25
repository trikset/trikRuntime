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

#pragma once

#include <QtCore/qglobal.h>

#include <QtCore/QString>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QHBoxLayout>
	#include <QtGui/QStackedLayout>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QHBoxLayout>
	#include <QtWidgets/QStackedLayout>
#endif

#include "mainWidget.h"
#include "controller.h"
#include "batteryIndicator.h"
#include "startWidget.h"

namespace trikGui {

/// TrikGui backround widget which is a parent for other trikGui widgets.
/// It consists of a status bar and a place for one of main widgets which is
/// used for conversation with user.
class BackgroundWidget : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configPath - path to a directory with config files (config.xml and so on). It is expected to be
	///        ending with "/".
	/// @param startDirPath - path to the directory from which the application was executed (it is expected to be
	///        ending with "/").
	/// @param parent - parent of this widget in terms of Qt parent-child widget relations.
	explicit BackgroundWidget(QString const &configPath, QString const &startDirPath, QWidget *parent = 0);

private:
	Controller mController;
	QVBoxLayout mMainLayout;
	QHBoxLayout mStatusBarLayout;
	QStackedLayout mMainWidgetsLayout;
	BatteryIndicator mBatteryIndicator;
	StartWidget mStartWidget;

public slots:
	/// Add a widget to main widgets layout and show it.
	/// @param widget - reference to the widget.
	void addMainWidget(MainWidget &widget);

	/// Remove a runningWidget from main widgets layout
	/// @param widget - reference to the runningWidget
	void closeRunningWidget(MainWidget &widget);

private slots:
	void renewFocus();
};

}
