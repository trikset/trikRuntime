/* Copyright 2014 CyberTech Labs Ltd.
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

#include "informationWidget.h"
#include <trikKernel/version.h>

#include <QtGui/QKeyEvent>
#include <QtNetwork/QNetworkInterface>

using namespace trikGui;

InformationWidget::InformationWidget(QWidget *parent)
	: TrikGuiDialog(parent)
{
	QLabel* const versionLabel = new QLabel(tr("Current version") + ": \n" + trikKernel::version);
	versionLabel->setAlignment(Qt::AlignTop);
	mLayout.addWidget(versionLabel);

	mUpdateButton = new QPushButton(tr("Update"));
	mLayout.addWidget(mUpdateButton);
	mUpdateButton->setDefault(true);

	QString mac = tr("Not found");

	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface &interface : interfaces) {
		if (interface.name() == "wlan0") {
			mac = interface.hardwareAddress();
			break;
		}
	}

	QLabel* const macLabel = new QLabel(tr("MAC address") + ": \n" + mac);
	mLayout.addWidget(macLabel);

	mLayout.addStretch();

	setLayout(&mLayout);
	setFocusPolicy(Qt::StrongFocus);
}

InformationWidget::~InformationWidget()
{
	delete mUpdateButton;
}

void InformationWidget::renewFocus()
{
}

QString InformationWidget::menuEntry()
{
	return QString(tr("Information"));
}

void InformationWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return: {
			updateVersion();
			break;
		}
		default: {
			TrikGuiDialog::keyPressEvent(event);
			break;
		}
	}
}

void InformationWidget::updateVersion()
{
	QMessageBox updateMessageBox(this);
	updateMessageBox.setIcon(QMessageBox::Question);
	updateMessageBox.setText(tr("Do you really want to update current version?"));
	updateMessageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	updateMessageBox.setDefaultButton(QMessageBox::Yes);
	const int answer = updateMessageBox.exec();

	switch (answer) {
		case QMessageBox::Yes: {
			UpdateWidget updateWidget;
			emit newWidget(updateWidget);
			updateWidget.exec();
			break;
		}
		default:
			break;
	 }

}
