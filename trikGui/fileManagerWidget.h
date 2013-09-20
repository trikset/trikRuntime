/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QFileSystemModel>
#include <QtCore/QString>

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

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	void showCurrentDir();
	void open();

	QVBoxLayout mLayout;
	QLabel mCurrentPathLabel;
	QListView mFileSystemView;
	QFileSystemModel mFileSystemModel;
	Controller &mController;
};

}
