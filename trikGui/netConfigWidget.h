/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <QtCore/QList>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QListView>

namespace trikGui {

/// Widget that shows current network configuration information like IP address.
class NetConfigWidget : public QWidget
{
	Q_OBJECT

public:
	explicit NetConfigWidget(QWidget *parent = 0);
	~NetConfigWidget();

	static QString menuEntry();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	QLabel mTitleLabel;
	QListView mConfigView;
	QList<QStandardItem *> mConfigItems;
	QStandardItemModel mConfigModel;
	QVBoxLayout mLayout;

	void generateNetConfigList();
};

}
