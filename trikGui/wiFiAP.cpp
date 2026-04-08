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

#include "wiFiAP.h"

#include <QtCore/QList>
#include <QDir>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>

#include <trikKernel/paths.h>
#include <trikKernel/rcReader.h>
#include <trikKernel/fileUtils.h>
#include <QrCodeGenerator.h>

using namespace trikGui;

WiFiAP::WiFiAP(QObject *parent) : QObject(parent) {
	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
	if (!name.isEmpty()) {
		mNetworkName = name;
		Q_EMIT networkNameChanged();
	}

	const trikKernel::RcReader rcReader(trikKernel::Paths::trikRcName());

	const QString passphrase = rcReader.value("trik_wifi_ap_passphrase");
	if (!passphrase.isEmpty()) {
		mPassword = passphrase;
		Q_EMIT passwordChanged();
	}

	const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface &interface : interfaces) {
		if (interface.name() == "wlan0") {
			const QList<QNetworkAddressEntry> entries = interface.addressEntries();
			for (const QNetworkAddressEntry &entry : entries) {
				const QHostAddress ip = entry.ip();
				if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
					mIpValue = ip.toString();
					Q_EMIT ipValueChanged();
					break;
				}
			}

			break;
		}
	}

	if (!mNetworkName.isEmpty() && !mPassword.isEmpty() && !mIpValue.isEmpty()) {
		QrCodeGenerator generator;
		QString data = "WIFI:T:WPA;S:" + mNetworkName + ";P:" + mPassword + ";H:;";
		QImage qrCodeImage = generator.generateQr(data, 300);
		QDir dir(trikKernel::Paths::imagesPath());

		if (!dir.exists() && !dir.mkpath(trikKernel::Paths::imagesPath())) {
			QLOG_ERROR() << "Cannot create directory for images";
		} else {
			const QString name = trikKernel::Paths::imagesPath() + "qrCodeImg.png";
			qrCodeImage.save(name);
			mNameImage = name;
			Q_EMIT nameImageChanged();
		}
	}
}

QString WiFiAP::networkName() { return mNetworkName; }
QString WiFiAP::password() { return mPassword; }
QString WiFiAP::ipValue() { return mIpValue; }
void WiFiAP::setQmlParent(QObject *parent) { setParent(parent); }

QString WiFiAP::nameImage() { return mNameImage; }