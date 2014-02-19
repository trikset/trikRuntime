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
#include <QtCore/QEventLoop>

#include "controller.h"

namespace trikGui {

/// File system browser able to launch executable files and .qts scripts.
class FileManagerWidget : public QWidget
{
	Q_OBJECT

public:
	explicit FileManagerWidget(Controller &controller, QWidget *parent = 0);
	~FileManagerWidget();

	static QString menuEntry();

	/// Show the widget and wait until it will be closed by user.
	void exec();

protected:
	void keyPressEvent(QKeyEvent *event);

private slots:
	void onDirectoryLoaded(QString const &path);

private:
	void showCurrentDir();
	void open();
	void renewCurrentIndex();

	QVBoxLayout mLayout;
	QLabel mCurrentPathLabel;
	QListView mFileSystemView;
	QFileSystemModel mFileSystemModel;
	Controller &mController;
	QString mCurrentDir;
	QEventLoop mEventLoop;
};

}
