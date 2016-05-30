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
{
	QStringList commandsList;
	commandsList
		<< tr("Play Tone")
		<< tr("Smile")
		<< tr("Sad Smile")
		<< tr("Timer")
		<< tr("Motor Forward")
		<< tr("Motor Backward")
		<< tr("Motors Stop")
		<< tr("Wait for Light")
		<< tr("Wait for Ultrasonic Distance")
		<< tr("Wait for Infrared Distance")
		<< tr("Wait for Encoder");

	for (int i = 0; i < commandsList.size(); ++i) {
		mCommands.addItem(commandsList.at(i));
		mCommands.item(i)->setData(Qt::UserRole, i);
	}

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
		const QVariant data = mCommands.currentItem()->data(Qt::UserRole);

		switch (data.toInt()) {
		case 0: {
			mValue = mCommands.currentItem()->text();
			mScript = QString("    brick.playSound(\"media/beep.wav\");\n");
			break;
		}
		case 1: {
			mValue = mCommands.currentItem()->text();
			mScript = QString("    brick.smile();\n");
			break;
		}
		case 2: {
			mValue = mCommands.currentItem()->text();
			mScript = QString("    brick.sadSmile();\n");
			break;
		}
		case 3: {
			const QString title(tr("Choose waiting time (ms):"));
			CommandSettingsWidget commandSettingsWidget(title, 5);
			emit newWidget(commandSettingsWidget);
			commandSettingsWidget.exec();

			const int value = commandSettingsWidget.value();
			mValue = tr("Delay %1 ms").arg(value);
			mScript = QString("    script.wait(%1);\n").arg(value);
			break;
		}
		case 4: {
			motorBehaviour();
			break;
		}
		case 5: {
			motorBehaviour();
			break;
		}
		case 6: {
			mValue = mCommands.currentItem()->text();
			mScript = QString();

			for (int i = 1; i < 5; ++i) {
				mScript.append(QString("    brick.motor(M%1).powerOff();\n").arg(i));
			}

			break;
		}
		case 7: {
			sensorBehaviour(QString("A1"), false);
			break;
		}
		case 8: {
			sensorBehaviour(QString("D1"), false);
			break;
		}
		case 9: {
			sensorBehaviour(QString("A1"), false);
			break;
		}
		case 10: {
			const QString port = mController.brick().encoderPorts().first();
			sensorBehaviour(port, true);
			break;
		}
		default: {
			mValue = tr("< add command >");
			break;
		}
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

QString CommandsListWidget::value() const
{
	return mValue;
}

QString CommandsListWidget::script() const
{
	return mScript;
}

void CommandsListWidget::motorBehaviour()
{
	const QString title(tr("Choose motors power (%): "));
	CommandSettingsWidget commandSettingsWidget(title, 2);
	emit newWidget(commandSettingsWidget);
	commandSettingsWidget.exec();

	mValue = mCommands.currentItem()->text();

	int power = commandSettingsWidget.value();
	if (mCommands.currentItem()->data(Qt::UserRole) == QString("Motor Backward")) {
		power *= -1;
	}

	mScript = QString();
	for (int i = 0; i < 4; ++i) {
		mScript.append(QString("    brick.motor(M%1).setPower(%2);\n").arg(i + 1).arg(power));
	}
}

void CommandsListWidget::sensorBehaviour(const QString &port, bool isEncoder)
{
	SensorSettingsWidget sensorSettingsWidget(port, isEncoder);
	emit newWidget(sensorSettingsWidget);
	sensorSettingsWidget.exec();

	mValue = mCommands.currentItem()->text();
	mScript = sensorSettingsWidget.createScript();
}
