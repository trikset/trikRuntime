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

#include "sensorsWidget.h"

using namespace trikGui;

SensorsSelectionWidget::SensorsSelectionWidget(trikControl::BrickInterface &brick
		, SensorType type
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("Select sensors for testing:"))
	, mSensorType(type)
	, mBrick(brick)
{
    QListWidgetItem *item = new QListWidgetItem(tr("Start testing"), &mList);
    item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);

	QStringList ports;

	switch (type) {
	case SensorType::analogSensor: {
		ports = mBrick.sensorPorts(trikControl::SensorInterface::Type::analogSensor);
		break;
	}
	case SensorType::digitalSensor: {
		ports = mBrick.sensorPorts(trikControl::SensorInterface::Type::digitalSensor);
		break;
	}
	case SensorType::encoder: {
		ports = mBrick.encoderPorts();
		break;
	}
	}

	ports.sort();

	for (const QString &port : ports) {
		QListWidgetItem *item = new QListWidgetItem(port, &mList);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
	}

	mList.setSelectionMode(QAbstractItemView::SingleSelection);
	mList.setFocusPolicy(Qt::StrongFocus);
	mList.setFocus();
	mList.setCurrentRow(0);

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mList);

	setLayout(&mLayout);
}

QString SensorsSelectionWidget::menuEntry(SensorType type)
{
	switch (type) {
	case SensorType::analogSensor: {
		return tr("Test analog sensors");
	}
	case SensorType::digitalSensor: {
		return tr("Test digital sensors");
	}
	case SensorType::encoder: {
		return tr("Test encoders");
	}
	}

	return QString();
}

void SensorsSelectionWidget::renewFocus()
{
	mList.setFocus();
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
	const int itemsCount = mList.count();
	for (int i = 0; i < itemsCount; ++i) {
		const QListWidgetItem &item = *mList.item(i);
		if ((item.flags() & Qt::ItemIsUserCheckable)
				&& (item.checkState() == Qt::Checked))
		{
			ports.append(item.text());
		}
	}

	const auto sensorType = mSensorType == SensorType::analogSensor || mSensorType == SensorType::digitalSensor
			? SensorsWidget::SensorType::analogOrDigitalSensor
			: SensorsWidget::SensorType::encoder
			;

	SensorsWidget sensorsWidget(mBrick, ports, sensorType);
	emit newWidget(sensorsWidget);
	if (sensorsWidget.exec() == TrikGuiDialog::goHomeExit) {
		goHome();
	}
}
