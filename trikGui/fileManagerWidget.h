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

#include <QVBoxLayout>
#include <QLabel>
#include <QListView>
#include <QFileSystemModel>
#include <QFileIconProvider>

#include "fileSystemFilter.h"
#include "controller.h"
#include "trikGuiDialog.h"

namespace trikGui {

/// File system browser able to launch executable files and .qts scripts.
class FileManagerWidget : public TrikGuiDialog
{
	Q_OBJECT

	/// Default QFileIconProvider spend about 10 seconds for scanning empty directory for file icons (Qt 5.8.0),
	/// so here is suggested the light overwrite version of QFileIconProvider class.
	/// In future it can be used to divide icons for ".py" and ".js" files.
	class LightFileIconProvider : public QFileIconProvider {
	public:
		/// Ligth version of icon method
		QIcon icon(IconType) const override;

		/// Ligth version of icon method
		QIcon icon(const QFileInfo &) const override;

		/// Ligth version of type method
		QString type(const QFileInfo &) const override;
	};

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
	void remove();
	void removeAll();
	QString currentPath();

	QVBoxLayout mLayout;
	QLabel mCurrentPathLabel;
	QListView mFileSystemView;
	QScopedPointer<LightFileIconProvider> mFileIconProvider;
	QFileSystemModel mFileSystemModel;
	Controller &mController;
	QString mRootDirPath;
	QString mLastSelectedFile;
	QString mDeleteAllFilesPath;
	QString mDeleteAllFilesName;
	FileSystemFilter mFilterProxyModel;
};

}
