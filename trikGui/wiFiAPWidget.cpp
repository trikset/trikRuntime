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

#include "wiFiAPWidget.h"

#include <QtCore/QDebug>
#include <QtCore/QList>

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>

#include "rcReader.h"

using namespace trikGui;

WiFiAPWidget::WiFiAPWidget(QWidget *parent)
	: TrikGuiDialog(parent)
{
	mTitle.setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	mParametersLayout.setAlignment(Qt::AlignCenter);
	mNetworkLabel.setAlignment(Qt::AlignCenter);
	mKeyLabel.setAlignment(Qt::AlignCenter);
	mIpLabel.setAlignment(Qt::AlignCenter);

	mTitle.setText(tr("Network parameters"));
	mNetworkLabel.setText(tr("Name not found"));
	mKeyLabel.setText(tr("Password not found"));
	mIpLabel.setText(tr("IP address not found"));

	getParameters();

	mMainLayout.addWidget(&mTitle);
	mMainLayout.addStretch(2);
	mMainLayout.addLayout(&mParametersLayout);
	mMainLayout.addStretch(1);

	mParametersLayout.addWidget(&mNetworkLabel);
	mParametersLayout.addWidget(&mKeyLabel);
	mParametersLayout.addWidget(&mIpLabel);

	setLayout(&mMainLayout);
}

void WiFiAPWidget::getParameters()
{
	RcReader const rcReader("/etc/trik/trikrc");

	QString const ssid = rcReader.value("trik_wifi_ap_ssid");
	if (!ssid.isEmpty()) {
		mNetworkLabel.setText(tr("Name: ") + ssid);
	}

	QString const passphrase = rcReader.value("trik_wifi_ap_passphrase");
	if (!passphrase.isEmpty()) {
		mKeyLabel.setText(tr("Password: ") + passphrase);
	}

	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	foreach (QNetworkInterface const &interface, interfaces) {
		if (interface.name() == "wlan0") {
			QList<QNetworkAddressEntry> entries = interface.addressEntries();
			foreach (QNetworkAddressEntry const &entry, entries) {
				QHostAddress ip = entry.ip();
				if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
					mIpLabel.setText(tr("IP address: ") + ip.toString());
					break;
				}
			}
			break;
		}
	}
}
