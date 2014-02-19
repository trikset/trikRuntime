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

#include <QtCore/QStringList>

#include "sensorsSelectionWidget.h"
#include "trikGuiApplication.h"

using namespace trikGui;

SensorsSelectionWidget::SensorsSelectionWidget(const QString &configPath
		, trikControl::Sensor::Type type
		, QWidget *parent)
	: QWidget(parent)
	, mTitle(tr("Select sensors for testing:"))
	, mBrick(*TrikGuiApplication::instance()->thread(), configPath)
{
	setWindowState(Qt::WindowFullScreen);

	QStringList ports = mBrick.sensorPorts(type);
	foreach (QString const &port, ports) {
		QListWidgetItem *item = new QListWidgetItem(tr("Sensor on") + " " + port, &mList);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
	}

	QListWidgetItem *item = NULL;

	item = new QListWidgetItem(tr("Start testing"), &mList);
	item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);

	mList.setSelectionMode(QAbstractItemView::SingleSelection);
	mList.setFocusPolicy(Qt::StrongFocus);
	mList.setFocus();
	mList.setCurrentRow(0);

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mList);

	setLayout(&mLayout);
}

void SensorsSelectionWidget::exec()
{
	show();
	mEventLoop.exec();
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
	if (event->key() == Qt::Key_Return) {
		QListWidgetItem *item = mList.currentItem();
		if (item->flags() & Qt::ItemIsUserCheckable) {
			if (item->checkState() == Qt::Checked) {
				item->setCheckState(Qt::Unchecked);
			} else {
				item->setCheckState(Qt::Checked);
			}
		} else if (item->text() == tr("Start testing")) {
			QList<int> ports;
			int const itemsCount = mList.count();
			for (int i = 0; i < itemsCount; ++i) {
				if ((mList.item(i)->flags() & Qt::ItemIsUserCheckable)
						&& (mList.item(i)->checkState() == Qt::Checked)) {
					ports.append(i + 1);
				}
			}

			// Start testing
		}
	} else {
		return QWidget::keyPressEvent(event);
	}
}
