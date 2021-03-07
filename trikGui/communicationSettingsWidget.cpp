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
#include <QtGui/QKeyEvent>

using namespace trikGui;

CommunicationSettingsWidget::CommunicationSettingsWidget(trikNetwork::MailboxInterface &mailbox
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("<b>Comm settings</b>"))
	, mSelectorsHelpLabel(tr("(Press 'Enter' to edit)"))
	, mHullNumberLabel(tr("Hull number:"))
	, mHullNumberSelector(mailbox.myHullNumber(), 2, 0, 35, this)
	, mServerIpLabel(tr("Leader IP:"))
	, mIpHelpLabel(tr("(last two numbers)"))
	, mServerIpSelector(0, 6, 3, 25, this)
	, mMailbox(mailbox)
{
	mConnectButton.setText(tr("Connect"));
	mTitle.setAlignment(Qt::AlignCenter);
	mHullNumberSelector.setFocus();

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mSelectorsHelpLabel);
	mLayout.addWidget(&mHullNumberLabel);
	mLayout.addWidget(&mHullNumberSelector);
	mLayout.addWidget(&mServerIpLabel);
	mLayout.addWidget(&mIpHelpLabel);
	mLayout.addWidget(&mServerIpSelector);
	mLayout.addWidget(&mConnectButton);

	QFont helpFont = mSelectorsHelpLabel.font();
	helpFont.setPixelSize(helpFont.pixelSize() - 7);
	mSelectorsHelpLabel.setFont(helpFont);
	mIpHelpLabel.setFont(helpFont);

	mSelectorsHelpLabel.setAlignment(Qt::AlignHCenter);

	const auto hostAddressString = mailbox.serverIp();

	if (hostAddressString == QHostAddress().toString()) {
		mServerIpSelector.setValue(0);
	} else {
		const auto parsedAddress = hostAddressString.split('.');
		Q_ASSERT(parsedAddress.size() == 4);
		auto hostAddress = parsedAddress[2].toInt() * 1000 + parsedAddress[3].toInt();
		mServerIpSelector.setValue(hostAddress);
	}

	setLayout(&mLayout);

	mConnectButton.setAutoFillBackground(true);
	connect(&mConnectButton, &ConnectButton::clicked, this, &CommunicationSettingsWidget::onConnectButtonClicked);
	connect(&mConnectButton, &ConnectButton::upPressed, this, &CommunicationSettingsWidget::focusUp);
	connect(&mConnectButton, &ConnectButton::downPressed, this, &CommunicationSettingsWidget::focusDown);

	connect(&mHullNumberSelector, &NumberSelectionWidget::valueChanged,
		this, &CommunicationSettingsWidget::onHullNumberChanged);

	connect(&mHullNumberSelector, &NumberSelectionWidget::upPressed, this, &CommunicationSettingsWidget::focusUp);
	connect(&mHullNumberSelector, &NumberSelectionWidget::downPressed,
		this, &CommunicationSettingsWidget::focusDown);

	connect(&mServerIpSelector,  &NumberSelectionWidget::upPressed, this, &CommunicationSettingsWidget::focusUp);
	connect(&mServerIpSelector,  &NumberSelectionWidget::downPressed,
		this, &CommunicationSettingsWidget::focusDown);
}

QString CommunicationSettingsWidget::menuEntry()
{
	return QString(tr("Comm settings"));
}

void CommunicationSettingsWidget::renewFocus()
{
	const QColor buttonColor = mMailbox.isConnected()
			? QColor(Qt::green)
			: QPalette().color(QPalette::Background);
	QPalette palette;
	palette.setColor(QPalette::Background, buttonColor);
	palette.setColor(QPalette::Base, buttonColor);
	palette.setColor(QPalette::Button, buttonColor);
	mConnectButton.setPalette(palette);
}

void CommunicationSettingsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up: {
		focusUp();
		event->accept();
		break;
	}
	case Qt::Key_Down: {
		focusDown();
		event->accept();
		break;
	}
	case Qt::Key_Return: {
		mConnectButton.animateClick();
		event->accept();
		break;
	}
	default: {
		TrikGuiDialog::keyPressEvent(event);
	}
	}
}

void CommunicationSettingsWidget::onConnectButtonClicked()
{
	mMailbox.renewIp();
	QStringList result = mMailbox.myIp().split('.');

	if (result.size() != 4) {
		/// @todo Properly notify user that the robot is not connected.
		return;
	}

	const QString ipSelectorValue = QString("%1").arg(mServerIpSelector.value(), 6, 10, QChar('0'));
	const QString thirdPart = ipSelectorValue.left(3).replace(QRegExp("^0+"), "");
	const QString fourthPart = ipSelectorValue.mid(3).replace(QRegExp("^0+"), "");
	result[2] = thirdPart.isEmpty() ? "0" : thirdPart;
	result[3] = fourthPart.isEmpty() ? "0" : fourthPart;
	mMailbox.connect(result.join("."));
}

void CommunicationSettingsWidget::onHullNumberChanged(int newHullNumber)
{
	mMailbox.setHullNumber(newHullNumber);
}

void CommunicationSettingsWidget::focusUp()
{
	if (mHullNumberSelector.hasFocusInside()) {
		mConnectButton.setFocus();
	} else if (mServerIpSelector.hasFocusInside()) {
		mHullNumberSelector.setFocus();
	} else {
		mServerIpSelector.setFocus();
	}
}

void CommunicationSettingsWidget::focusDown()
{
	if (mHullNumberSelector.hasFocusInside()) {
		mServerIpSelector.setFocus();
	} else if (mServerIpSelector.hasFocusInside()) {
		mConnectButton.setFocus();
	} else {
		mHullNumberSelector.setFocus();
	}
}
