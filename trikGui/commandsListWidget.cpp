/* Copyright 2016 Mikhail Kita
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

#include "commandsListWidget.h"

#include <QtGui/QKeyEvent>
#include <trikKernel/paths.h>
#include <QsLog.h>

#include "commandSettingsWidget.h"
#include "sensorSettingsWidget.h"
#include "motorsWidget.h"

using namespace trikGui;
using trikControl::MotorInterface;

CommandsListWidget::CommandsListWidget(Controller &controller, const QString &name, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("Choose command:"))
	, mController(controller)
	, mValue(name)
	, mScript("")
{
	mCommands.addItem(tr("Clear"));
	mCommands.addItem(tr("Play Tone"));
	mCommands.addItem(tr("Smile"));
	mCommands.addItem(tr("Sad Smile"));
	mCommands.addItem(tr("Timer"));
	mCommands.addItem(tr("Motor Forward"));
	// mCommands.addItem(tr("Motor Backward"));
	mCommands.addItem(tr("Motors Stop"));
	mCommands.addItem(tr("Wait for Light"));
	mCommands.addItem(tr("Wait for Ultrasonic Distance"));
	mCommands.addItem(tr("Wait for Infrared Distance"));

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mCommands);

	mCommands.selectionModel()->select(
		mCommands.model()->index(0, 0)
		, QItemSelectionModel::ClearAndSelect
	);

	setLayout(&mLayout);
}

QString CommandsListWidget::menuEntry()
{
	return tr("Commands List");
}

void CommandsListWidget::renewFocus()
{
	mCommands.setFocus();
}

void CommandsListWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Return: {
		if (mCommands.currentItem()->text() == "Play Tone") {
			mValue = mCommands.currentItem()->text();
			mScript = QString("    brick.playSound(\"media/beep.wav\");\n");

		} else if (mCommands.currentItem()->text() == "Smile") {
			mValue = mCommands.currentItem()->text();
			mScript = QString("    brick.smile();\n");

		} else if (mCommands.currentItem()->text() == "Sad Smile") {
			mValue = mCommands.currentItem()->text();
			mScript = QString("    brick.sadSmile();\n");

		} else if (mCommands.currentItem()->text() == "Timer") {
			QString title("Choose waiting time (ms):");
			CommandSettingsWidget commandSettingsWidget(title, 5);
			emit newWidget(commandSettingsWidget);
			commandSettingsWidget.exec();

			const int value = commandSettingsWidget.value();
			mValue = QString("Delay %1 ms").arg(value);
			mScript = QString("    script.wait(%1);\n").arg(value);

		} else if (mCommands.currentItem()->text() == "Motors Stop") {
			mValue = mCommands.currentItem()->text();
			mScript = QString("");

			for (int i = 0; i < 4; ++i) {
				mScript.append(QString("    brick.motor(M%1).powerOff();\n").arg(i + 1));
			}

		} else if (mCommands.currentItem()->text().startsWith("Motor")) {
			// TODO: choose the best variant.
			motorBehaviour1();
			// motorBehaviour2();

		} else if (mCommands.currentItem()->text().startsWith("Wait")) {
			QString port("A1");
			if (mCommands.currentItem()->text() == "Wait for Ultrasonic Distance") {
				port = "D1";
			}

			SensorSettingsWidget sensorSettingsWidget(port);
			emit newWidget(sensorSettingsWidget);
			sensorSettingsWidget.exec();

			mValue = mCommands.currentItem()->text();
			mScript = sensorSettingsWidget.createScript();

		} else {
			mValue = QString("< add command >");
		}
		exit();
		break;
	}
	default: {
		TrikGuiDialog::keyPressEvent(event);
		break;
	}
	}
}

const QString CommandsListWidget::value() 
{
	return mValue;
}

const QString CommandsListWidget::script()
{
	return mScript;
}

void CommandsListWidget::motorBehaviour1()
{
	MotorsWidget motorsWidget(mController.brick(), MotorInterface::Type::powerMotor);
	emit newWidget(motorsWidget);
	motorsWidget.exec();

	mValue = mCommands.currentItem()->text();
	std::vector<int> data = motorsWidget.data();
	mScript = QString("");

	for (int i = 0; i < 4; ++i) {
		mScript.append(QString("    brick.motor(M%1).setPower(%2);\n").arg(i + 1).arg(data[i]));
	}
}

void CommandsListWidget::motorBehaviour2()
{
	QString title("Choose motors power (%): ");
	CommandSettingsWidget commandSettingsWidget(title, 2);
	emit newWidget(commandSettingsWidget);
	commandSettingsWidget.exec();

	mValue = mCommands.currentItem()->text();

	int data = commandSettingsWidget.value();
	if (mCommands.currentItem()->text() == "Motor Backward") {
		data *= -1;
	}
	mScript = QString("");

	for (int i = 0; i < 4; ++i) {
		mScript.append(QString("    brick.motor(M%1).setPower(%2);\n").arg(i + 1).arg(data));
	}
}
