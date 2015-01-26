/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QLabel>
#endif

#include <trikNetwork/mailboxInterface.h>

#include "trikGuiDialog.h"
#include "numberSelectionWidget.h"
#include "connectButton.h"

namespace trikGui {

/// Widget that is used to configure settings of robot group communication via mailboxes.
class CommunicationSettingsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param mailbox - reference to mailbox object which we will configure here.
	/// @param parent - parent of this widget in terms of Qt widget parent/child system.
	explicit CommunicationSettingsWidget(trikNetwork::MailboxInterface &mailbox, QWidget *parent = 0);

	/// String that shall appear in menu for this widget.
	static QString menuEntry();

public slots:
	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	/// Called when 'Connect' button is pressed, initiates connection to leader robot.
	void onConnectButtonClicked();

	/// Called when hull number of this robot is changed. Hull number is stored in local settings, but used only after
	/// reconnect.
	void onHullNumberChanged(int newHullNumber);

	/// Called by child widgets when they need to move input focus one widget up.
	void focusUp();

	/// Called by child widgets when they need to move input focus one widget down.
	void focusDown();

private:
	/// Main layout of this widget.
	QVBoxLayout mLayout;

	/// Title label.
	QLabel mTitle;

	/// Help about widget number selectors.
	QLabel mSelectorsHelpLabel;

	/// Label for hull number selector.
	QLabel mHullNumberLabel;

	/// Hull number selector widget.
	NumberSelectionWidget mHullNumberSelector;

	/// Label for server IP selector.
	QLabel mServerIpLabel;

	/// Help about server IP selector.
	QLabel mIpHelpLabel;

	/// Selector that allows to select last 6 digits of lead robot IP.
	NumberSelectionWidget mServerIpSelector;

	/// Button that initiates connection to lead robot.
	ConnectButton mConnectButton;

	/// Reference to mailbox object.
	trikNetwork::MailboxInterface &mMailbox;
};

}
