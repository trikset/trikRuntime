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

#include <QObject>
#include <trikNetwork/mailboxInterface.h>

namespace trikGui {

/// Widget that is used to configure settings of robot group communication via mailboxes.
class CommunicationSettings : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString hullNumber READ hullNumber CONSTANT)
	Q_PROPERTY(QString thirdFourthIpPart READ thirdFourthIpPart CONSTANT)

public:
	/// Constructor.
	/// @param mailbox - reference to mailbox object which we will configure here.
	/// @param parent - parent of this widget in terms of Qt widget parent/child system.
	explicit CommunicationSettings(trikNetwork::MailboxInterface &mailbox, QObject *parent = nullptr);

	Q_INVOKABLE void setQmlParent(QObject *parent);

	/// Called when 'Connect' button is pressed, initiates connection to leader robot.
	Q_INVOKABLE void connectToLeader(QString newThirdFourthIpPart);

	/// Called when hull number of this robot is changed. Hull number is stored in local settings, but used only after
	/// reconnect.
	Q_INVOKABLE void setHullNumber(int newHullNumber);

private:
	/// Reference to mailbox object.
	trikNetwork::MailboxInterface &mMailbox;

	int mHullNumber;

	int mThirdFourthIpPart;

	QString hullNumber();
	QString thirdFourthIpPart();
};

}
