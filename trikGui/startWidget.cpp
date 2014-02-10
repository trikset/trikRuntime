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

#include "startWidget.h"

#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include "fileManagerWidget.h"
#include "netConfigWidget.h"

using namespace trikGui;

StartWidget::StartWidget(QString const &configPath, QWidget *parent)
	: QWidget(parent)
	, mController(configPath)
	, mConfigPath(configPath)
{
	setWindowState(Qt::WindowFullScreen);

	mTitleLabel.setText(tr("TRIK"));

	mMenuModel.appendRow(new QStandardItem(FileManagerWidget::menuEntry()));
	mMenuModel.appendRow(new QStandardItem(NetConfigWidget::menuEntry()));

	QStandardItem * const settingsItem = new QStandardItem(tr("Settings"));
	mMenuModel.appendRow(settingsItem);
	settingsItem->appendRow(new QStandardItem("Empty 1"));
	settingsItem->appendRow(new QStandardItem("Empty 2"));

	mMenuView.setModel(&mMenuModel);

	mMenuView.selectionModel()->select(
			mMenuModel.index(0, 0)
			, QItemSelectionModel::ClearAndSelect
			);

	mLayout.addWidget(&mTitleLabel);
	mLayout.addWidget(&mMenuView);

	setLayout(&mLayout);
}

StartWidget::~StartWidget()
{
}

void StartWidget::launch()
{
	QModelIndex const &currentIndex = mMenuView.currentIndex();
	QStandardItem const * const currentItem = mMenuModel.itemFromIndex(currentIndex);
	if (currentItem->hasChildren()) {
		setRootIndex(currentIndex);
	} else {
		QString currentItemText = currentItem->text();
		if (currentItemText == FileManagerWidget::menuEntry()) {
			/// @todo Why widgets are created every time?
			FileManagerWidget *fileManagerWidget = new FileManagerWidget(mController, this);
			fileManagerWidget->show();
		} else if (currentItemText == NetConfigWidget::menuEntry()) {
			NetConfigWidget *netConfigWidget = new NetConfigWidget(mConfigPath, this);
			netConfigWidget->show();
		} else {
			qDebug() << currentItemText << "clicked";
		}
	}
}

void StartWidget::setRootIndex(QModelIndex const &index)
{
	QStandardItem const *item = mMenuModel.itemFromIndex(index);

	if (item == NULL) {
		item = mMenuModel.invisibleRootItem();
	}

	if (!item->hasChildren()) {
		return;
	}

	mSelections[mMenuView.rootIndex()] = mMenuView.currentIndex();

	mMenuView.setRootIndex(index);

	QModelIndex const &selectedItemIndex = mSelections.value(index, mMenuModel.indexFromItem(item->child(0)));

	mMenuView.selectionModel()->select(
			selectedItemIndex
			, QItemSelectionModel::ClearAndSelect
			);

	mMenuView.setCurrentIndex(selectedItemIndex);
}

void StartWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Meta: {
			setRootIndex(QModelIndex());
			break;
		}
		case Qt::Key_Left: {
			QStandardItem const * const rootItem = mMenuModel.itemFromIndex(mMenuView.rootIndex());
			if (rootItem == NULL) {
				break;
			}

			setRootIndex(mMenuModel.indexFromItem(rootItem->parent()));
			break;
		}
		case Qt::Key_Return: case Qt::Key_Right: {
			launch();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
