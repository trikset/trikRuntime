/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
#else
	#include <QtWidgets/QWidget>
#endif

namespace trikGui {

/// Abstract class for widgets which will be used for conversation with user.
class MainWidget : public QWidget
{
	Q_OBJECT

public:
	enum class FileManagerRootType {
		scriptsDir
		, allFS
	};

	/// Constructor.
	/// @param parent - parent of that widget in terms of Qt parent/child widget relations.
	MainWidget(QWidget *parent) : QWidget(parent) {}

	/// Set focus properly on this widget or one of its children.
	virtual void renewFocus() = 0;

signals:
	/// Emitted when the widget created a new one and wants it to be shown.
	void newWidget(trikGui::MainWidget &widget);
};
}
