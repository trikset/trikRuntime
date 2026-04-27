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

#include "communicationSettings.h"

#include <QtCore/QRegExp>
#include <QtGui/QKeyEvent>

using namespace trikGui;

CommunicationSettings::CommunicationSettings(trikNetwork::MailboxInterface &mailbox, QObject *parent)
	: QObject(parent), mMailbox(mailbox) {

	mHullNumber = mailbox.myHullNumber();
	const auto hostAddressString = mailbox.serverIp();

	if (hostAddressString == QHostAddress().toString()) {
		mThirdFourthIpPart = 0;
	} else {
		const auto parsedAddress = hostAddressString.split('.');
		Q_ASSERT(parsedAddress.size() == 4);
		auto hostAddress = parsedAddress[2].toInt() * 1000 + parsedAddress[3].toInt();

		mThirdFourthIpPart = hostAddress;
	}
}

static bool isValidOctet(const QString &s)
{
	bool ok;
	const int v = s.toInt(&ok);
	return ok && v >= 0 && v <= 255;
}

void CommunicationSettings::connectToLeader(const QString &newThirdFourthIpPart) {
	mMailbox.renewIp();
	QStringList result = mMailbox.myIp().split('.');
	QStringList thirdFourthIpPart = newThirdFourthIpPart.split('.');

	if (result.size() != 4 || thirdFourthIpPart.size() != 2) {
		Q_EMIT invalidIpEntered();
		return;
	}

	const QString thirdPart = thirdFourthIpPart[0].replace(QRegExp("^0+"), "");
	const QString fourthPart = thirdFourthIpPart[1].replace(QRegExp("^0+"), "");
	result[2] = thirdPart.isEmpty() ? "0" : thirdPart;
	result[3] = fourthPart.isEmpty() ? "0" : fourthPart;

	if (!isValidOctet(result[2]) || !isValidOctet(result[3])) {
		Q_EMIT invalidIpEntered();
		return;
	}

	mMailbox.connect(result.join("."));
}

QString CommunicationSettings::hullNumber() const {
	return QString("%1").arg(mHullNumber, 2, 10, QChar('0'));;
}

QString CommunicationSettings::thirdFourthIpPart() const {
	return QString("%1").arg(mThirdFourthIpPart, 6, 10, QChar('0')).insert(3, ".");;
}

void CommunicationSettings::setHullNumber(int newHullNumber) { mMailbox.setHullNumber(newHullNumber); }

void CommunicationSettings::setQmlParent(QObject *parent) { setParent(parent); }
