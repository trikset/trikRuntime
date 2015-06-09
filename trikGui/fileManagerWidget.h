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
 * limitations under the License. */

/* This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
	#include <QtGui/QListView>
	#include <QtGui/QFileSystemModel>
#else
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QListView>
	#include <QtWidgets/QFileSystemModel>
#endif

#include <QtCore/QString>

#include "controller.h"
#include "trikGuiDialog.h"
#include "fileManagerMessageBox.h"

namespace trikGui {

/// File system browser able to launch executable files and .qts scripts.
class FileManagerWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param controller - reference to controller object which provides access to low-level functionality.
	/// @param fileManagerRoot - root directory for file browser.
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit FileManagerWidget(Controller &controller, MainWidget::FileManagerRootType fileManagerRoot
			, QWidget *parent = 0);

	~FileManagerWidget() override;

	/// Returns main menu entry string for this widget.
	static QString menuEntry();

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	void onDirectoryLoaded(const QString &path);
	void renewCurrentIndex();
	void onSelectionChanged(QModelIndex current, QModelIndex previous);

private:
	void showCurrentDir();
	void open();
	QString currentPath();

	QVBoxLayout mLayout;
	QLabel mCurrentPathLabel;
	QListView mFileSystemView;
	QFileSystemModel mFileSystemModel;
	Controller &mController;
	QString mRootDirPath;
	QString mLastSelectedFile;

	FileManagerMessageBox mOpenDeleteBox;
};

}
