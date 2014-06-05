/* Copyright 2014 Roman Kurbatov
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

#include "sensorsSelectionWidget.h"

#include <QtCore/QStringList>

#include "trikGuiApplication.h"
#include "sensorsWidget.h"

using namespace trikGui;

SensorsSelectionWidget::SensorsSelectionWidget(const QString &configPath
		, trikControl::Sensor::Type type
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("Select sensors for testing:"))
	, mBrick(*TrikGuiApplication::instance()->thread(), configPath)
{
	QStringList ports = mBrick.sensorPorts(type);
	foreach (QString const &port, ports) {
		QListWidgetItem *item = new QListWidgetItem(port, &mList);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
	}

	QListWidgetItem *item = new QListWidgetItem(tr("Start testing"), &mList);
	item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);

	mList.setSelectionMode(QAbstractItemView::SingleSelection);
	mList.setFocusPolicy(Qt::StrongFocus);
	mList.setFocus();
	mList.setCurrentRow(0);

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mList);

	setLayout(&mLayout);
}

QString SensorsSelectionWidget::menuEntry(trikControl::Sensor::Type type)
{
	switch (type) {
		case trikControl::Sensor::analogSensor: {
			return tr("Test analog sensors");
		}
		case trikControl::Sensor::digitalSensor: {
			return tr("Test digital sensors");
		}
	}

	return QString();
}

void SensorsSelectionWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return: {
			activateItem();
			break;
		}
		default: {
			TrikGuiDialog::keyPressEvent(event);
		}
	}
}

void SensorsSelectionWidget::activateItem()
{
	QListWidgetItem *item = mList.currentItem();
	if (item->flags() & Qt::ItemIsUserCheckable) {
		if (item->checkState() == Qt::Checked) {
			item->setCheckState(Qt::Unchecked);
		} else {
			item->setCheckState(Qt::Checked);
		}
	} else if (item->text() == tr("Start testing")) {
		startTesting();
	}
}

void SensorsSelectionWidget::startTesting()
{
	QStringList ports;
	int const itemsCount = mList.count();
	for (int i = 0; i < itemsCount; ++i) {
		QListWidgetItem const &item = *mList.item(i);
		if ((item.flags() & Qt::ItemIsUserCheckable)
				&& (item.checkState() == Qt::Checked))
		{
			ports.append(item.text());
		}
	}

	SensorsWidget sensorsWidget(mBrick, ports);
	if (sensorsWidget.exec() == TrikGuiDialog::goHomeExit) {
		goHome();
	}
}
