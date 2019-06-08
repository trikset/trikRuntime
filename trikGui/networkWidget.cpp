/* Copyright 2019 CyberTech Labs Ltd.
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

#include "networkWidget.h"

#include <QWidget>
#include <QNetworkInterface>

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>

using namespace trikKernel;
using namespace trikGui;

NetworkWidget::NetworkWidget(QWidget *parent)
	: QWidget(parent)
{
	mHostnameLabel.setText("Name: ");
	mIPLabel.setText("IP: ");
	mNetworkLayout.addWidget(&mHostnameLabel);
	mNetworkLayout.addWidget(&mIPLabel);
	setLayout(&mNetworkLayout);

	mUpdateTimer.setInterval(5000);
	connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateIP()));
	connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateHostname()));
	mUpdateTimer.start();
}

void NetworkWidget::updateIP()
{
	const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface &interface : interfaces) {
		if (interface.name() == "wlan0") {
			const QList<QNetworkAddressEntry> entries = interface.addressEntries();
			for (const QNetworkAddressEntry &entry : entries) {
				const QHostAddress ip = entry.ip();
				if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
					mIPLabel.setText(tr("IP: ") + ip.toString());
					break;
				}
			}

			break;
		}
	}
}

void NetworkWidget::updateHostname()
{
	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
	mHostnameLabel.setText(tr("Name: ") + name);
}
