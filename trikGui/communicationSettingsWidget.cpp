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

#include "communicationSettingsWidget.h"

#include <QtCore/QRegExp>

using namespace trikGui;

CommunicationSettingsWidget::CommunicationSettingsWidget(trikControl::Brick &brick
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("<b>Comm settings</b>"))
	, mHullNumberLabel(tr("Hull number:"))
	, mHullNumberSelector(brick.mailbox()->hullNumber(), 2, 0, this)
	, mServerIpLabel(tr("Leader IP:"))
	, mServerIpSelector(0, 6, 3, this)
	, mBrick(brick)
{
	mConnectButton.setText(tr("Connect"));
	mTitle.setAlignment(Qt::AlignCenter);
	mHullNumberSelector.setFocus();

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mHullNumberLabel);
	mLayout.addWidget(&mHullNumberSelector);
	mLayout.addWidget(&mServerIpLabel);
	mLayout.addWidget(&mServerIpSelector);
	mLayout.addWidget(&mConnectButton);

	auto const hostAddressString = brick.mailbox()->serverIp().toString();
	auto const parsedAddress = hostAddressString.split('.');
	Q_ASSERT(parsedAddress.size() == 4);
	auto hostAddress = parsedAddress[2].toInt() * 1000 + parsedAddress[3].toInt();
	mServerIpSelector.setValue(hostAddress);

	setLayout(&mLayout);

	connect(&mConnectButton, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));
	connect(&mHullNumberSelector, SIGNAL(valueChanged(int)), this, SLOT(onHullNumberChanged(int)));
}

QString CommunicationSettingsWidget::menuEntry()
{
	return QString(tr("Comm settings"));
}

void CommunicationSettingsWidget::renewFocus()
{
}

void CommunicationSettingsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up: {
		if (mHullNumberSelector.hasFocusInside()) {
			mConnectButton.setFocus();
		} else if (mServerIpSelector.hasFocusInside()) {
			mHullNumberSelector.setFocus();
		} else {
			mServerIpSelector.setFocus();
		}

		event->accept();
		break;
	}
	case Qt::Key_Down: {
		if (mHullNumberSelector.hasFocusInside()) {
			mServerIpSelector.setFocus();
		} else if (mServerIpSelector.hasFocusInside()) {
			mConnectButton.setFocus();
		} else {
			mHullNumberSelector.setFocus();
		}

		event->accept();
		break;
	}
	case Qt::Key_Return: {
		mConnectButton.animateClick();
		event->accept();
		break;
	}
	default:
	{
		event->accept();
	}
	}
}

void CommunicationSettingsWidget::onConnectButtonClicked()
{
	QStringList result = mBrick.mailbox()->serverIp().toString().split('.');
	QString const ipSelectorValue = QString("%1").arg(mServerIpSelector.value(), 6, 10, QChar('0'));
	QString const thirdPart = ipSelectorValue.left(3).replace(QRegExp("^0+"), "");
	QString const fourthPart = ipSelectorValue.mid(3).replace(QRegExp("^0+"), "");
	result[2] = thirdPart;
	result[3] = fourthPart;
	mBrick.mailbox()->connect(result.join("."));
}

void CommunicationSettingsWidget::onHullNumberChanged(int newHullNumber)
{
	mBrick.mailbox()->setHullNumber(newHullNumber);
}
