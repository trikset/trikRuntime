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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QHBoxLayout>
	#include <QtGui/QLabel>
	#include <QtGui/QPushButton>
	#include <QtGui/QMessageBox>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QPushButton>
	#include <QtWidgets/QMessageBox>
#endif

#include "trikGuiDialog.h"
#include "updateWidget.h"

namespace trikGui {

/// Widget that shows current version of TrikRuntime.
class VersionWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent of this widget in terms of Qt widget parent/child system.
	explicit VersionWidget(QWidget *parent = 0);

	/// Destructor.
	~VersionWidget();

	/// String that shall appear in menu for this widget.
	static QString menuEntry();

public slots:
	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	void updateVersion();

private:
	/// Main layout of this widget.
	QHBoxLayout mLayout;
	QPushButton *mUpdateButton;  // Has ownership.
};

}
