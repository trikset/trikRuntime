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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include "trikControl/motor.h"
#include "fileManagerWidget.h"
#include "wiFiModeWidget.h"
#include "motorsWidget.h"
#include "sensorsSelectionWidget.h"

using namespace trikGui;

StartWidget::StartWidget(Controller &controller, QString const &configPath, QWidget *parent)
	: MainWidget(parent)
	, mController(controller)
	, mConfigPath(configPath)
{
	mTitleLabel.setText(tr("TRIK"));

	mMenuModel.appendRow(new QStandardItem(FileManagerWidget::menuEntry()));
	mMenuModel.appendRow(new QStandardItem(WiFiModeWidget::menuEntry()));

	QStandardItem * const settingsItem = new QStandardItem(tr("Settings"));
	mMenuModel.appendRow(settingsItem);
	settingsItem->appendRow(new QStandardItem(MotorsWidget::menuEntry(trikControl::Motor::powerMotor)));
	settingsItem->appendRow(new QStandardItem(MotorsWidget::menuEntry(trikControl::Motor::servoMotor)));
	settingsItem->appendRow(new QStandardItem(SensorsSelectionWidget::menuEntry(trikControl::Sensor::analogSensor)));
	settingsItem->appendRow(new QStandardItem(SensorsSelectionWidget::menuEntry(trikControl::Sensor::digitalSensor)));

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

void StartWidget::renewFocus()
{
	mMenuView.setFocus();
}

void StartWidget::launch()
{
	QModelIndex const &currentIndex = mMenuView.currentIndex();
	QStandardItem const * const currentItem = mMenuModel.itemFromIndex(currentIndex);
	if (currentItem->hasChildren()) {
		setRootIndex(currentIndex);
	} else {
		QString currentItemText = currentItem->text();

		int result = TrikGuiDialog::normalExit;

		if (currentItemText == FileManagerWidget::menuEntry()) {
			/// @todo Why widgets are created every time?
			FileManagerWidget fileManagerWidget(mController);
			emit newWidget(fileManagerWidget);
			result = fileManagerWidget.exec();
		} else if (currentItemText == WiFiModeWidget::menuEntry()) {
			WiFiModeWidget wiFiModeWidget(mConfigPath);
			emit newWidget(wiFiModeWidget);
			result = wiFiModeWidget.exec();
		} else if (currentItemText == MotorsWidget::menuEntry(trikControl::Motor::powerMotor)) {
			MotorsWidget motorsWidget(mController.brick(), trikControl::Motor::powerMotor);
			emit newWidget(motorsWidget);
			result = motorsWidget.exec();
		} else if (currentItemText == MotorsWidget::menuEntry(trikControl::Motor::servoMotor)) {
			MotorsWidget motorsWidget(mController.brick(), trikControl::Motor::servoMotor);
			emit newWidget(motorsWidget);
			result = motorsWidget.exec();
		} else if (currentItemText == SensorsSelectionWidget::menuEntry(trikControl::Sensor::analogSensor)) {
			SensorsSelectionWidget sensorsSelectionWidget(mController.brick(), trikControl::Sensor::analogSensor);
			emit newWidget(sensorsSelectionWidget);
			result = sensorsSelectionWidget.exec();
		} else if (currentItemText == SensorsSelectionWidget::menuEntry(trikControl::Sensor::digitalSensor)) {
			SensorsSelectionWidget sensorsSelectionWidget(mController.brick(), trikControl::Sensor::digitalSensor);
			emit newWidget(sensorsSelectionWidget);
			result = sensorsSelectionWidget.exec();
		}

		if (result == TrikGuiDialog::goHomeExit) {
			goHome();
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

	QModelIndex selectedItemIndex;
	if (mSelections.contains(index)) {
		selectedItemIndex = mSelections[index];
	} else {
		selectedItemIndex = mMenuModel.indexFromItem(item->child(0));
	}

	mMenuView.selectionModel()->select(
			selectedItemIndex
			, QItemSelectionModel::ClearAndSelect
			);

	mMenuView.setCurrentIndex(selectedItemIndex);
}

void StartWidget::goHome()
{
	setRootIndex(QModelIndex());
}

void StartWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_PowerDown: {
			goHome();
			break;
		}
		case Qt::Key_Escape: {
			QStandardItem const * const rootItem = mMenuModel.itemFromIndex(mMenuView.rootIndex());
			if (rootItem == NULL) {
				break;
			}

			setRootIndex(mMenuModel.indexFromItem(rootItem->parent()));
			break;
		}
		case Qt::Key_Return: {
			launch();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
