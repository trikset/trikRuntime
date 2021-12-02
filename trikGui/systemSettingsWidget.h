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
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QRadioButton>
	#include <QtGui/QButtonGroup>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QRadioButton>
	#include <QtWidgets/QButtonGroup>
#endif

#include "trikGuiDialog.h"

namespace trikGui {

/// Widget that shows some system settings.
class SystemSettingsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent of this widget in terms of Qt widget parent/child system.
	/// @param fileManagerRoot - current FilesManagerRoot
	explicit SystemSettingsWidget(MainWidget::FileManagerRootType fileManagerRoot, QWidget *parent = 0);

	/// Destructor.
	~SystemSettingsWidget();

	/// String that shall appear in menu for this widget.
	static QString menuEntry();

public slots:
	void renewFocus() override;

signals:
	/// Emitted when we want to save current FilesManagerRoot
	void currentFilesDirPath(MainWidget::FileManagerRootType const& path);

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	void changeDefaultButton();
	void emitCheckedDirPath();
	void setCurrentFilesRootButton(MainWidget::FileManagerRootType fileManagerRoot);

	/// Main layout of this widget.
	QButtonGroup mButtonGroup;
	QVBoxLayout mLayout;
	QRadioButton* mAllFSButton = nullptr;  // Has ownership.
	QRadioButton* mOnlyScriptsButton = nullptr;  // Has ownership.
};

}
