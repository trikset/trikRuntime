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

#include "network.h"

#include <QNetworkInterface>
#include <QtCore/QSettings>

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>

using namespace trikKernel;
using namespace trikGui;

Network::Network(QObject *parent) : QObject(parent) {
	mUpdateTimer.setInterval(5000);
	connect(&mUpdateTimer, &QTimer::timeout, this, &Network::updateIP);
	connect(&mUpdateTimer, &QTimer::timeout, this, &Network::updateHostname);
	connect(&mUpdateTimer, &QTimer::timeout, this, &Network::updateHullNumber);
	mUpdateTimer.start();
}

void Network::updateIP() {
	QString newIp = "";
	const auto &interfaces = QNetworkInterface::allInterfaces();
	const auto &interface =
		std::find_if(interfaces.begin(), interfaces.end(),
			 [](const QNetworkInterface &interface) { return interface.name() == "wlan0"; });
	if (interface != interfaces.end()) {
		const auto &entries = interface[0].addressEntries();
		const auto &entry = std::find_if(entries.begin(), entries.end(), [](const QNetworkAddressEntry &entry) {
			return entry.ip().protocol() == QAbstractSocket::IPv4Protocol;
		});
		if (entry != entries.end()) {
			newIp = entry[0].ip().toString();
		}
	}

	if (newIp != mIp) {
		mIp = newIp;
		Q_EMIT ipChanged();
	}
}

void Network::updateHostname() {
	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
	mHostName = name;
	Q_EMIT hostNameChanged();
}

void Network::updateHullNumber() {
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	auto hullNumber = settings.value("hullNumber");
	mHullNumber = QString::number(hullNumber.toInt());
	Q_EMIT hullNumberChanged();
}

QString Network::hostName() { return mHostName; }
QString Network::ip() { return mIp; }
QString Network::hullNumber() { return mHullNumber; }
