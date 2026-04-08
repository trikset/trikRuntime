/* Copyright 2024 Daniel Chehade.
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

#include "wiFiClientMock.h"
#include <QsLog.h>
#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include <trikWiFi/trikWiFi.h>
#include <QtGui/QKeyEvent>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QNetworkInterface>
#include <QtQml>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollBar>
#include <unistd.h>

using namespace trikGui;
using namespace trikWiFi;

WiFiClientMock::WiFiClientMock(QObject *parent)
	: QAbstractListModel(parent), mConnectionState(ConnectionState::NotConnected) {
	qmlRegisterUncreatableType<WiFiClientMock>("WiFiClient", 1, 0, "ConnectionState", "Enum is not a type");

	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
}

WiFiClientMock::~WiFiClientMock() {}

void WiFiClientMock::setQmlParent(QObject *parent) { setParent(parent); }

void WiFiClientMock::scanWiFi() { onNetworksInfoUpdated(); }

void WiFiClientMock::onNetworksInfoUpdated() {
	Q_EMIT beginResetModel();
	mAvailableNetworks.clear();

	mNetworks.clear();
	NetworkInfoMock network1{"wifi 1", false, trikWiFi::Security::wpa};
	NetworkInfoMock network2{"wifi 2", false, trikWiFi::Security::wep};
	NetworkInfoMock network3{"wifi 3", false, trikWiFi::Security::none};
	mNetworks.insert(network1.mSsid, network1);
	mAvailableNetworks.append(network1);

	mNetworks.insert(network2.mSsid, network2);
	mAvailableNetworks.append(network2);

	mNetworks.insert(network3.mSsid, network3);
	mAvailableNetworks.append(network3);

	for (int i = 0; i < 10; i++) {
		NetworkInfoMock network{"wifi " + QString::number(i), true, trikWiFi::Security::wpa};
		/// If two networks have the same ssid, only last one will be
		/// shown. Some routers boadcast ssids on different channels
		/// and they will be shown as different networks if we do not
		/// filter them here. It is perceived as a bug by users, so we
		/// sacrifice correctness in sake of simplicity.
		mNetworks.insert(network.mSsid, network);
		mAvailableNetworks.append(network);
	}

	// Q_EMIT availableNetworksChanged();
	isScanning = false;
	Q_EMIT endResetModel();
}

void WiFiClientMock::setConnectionStatus(ConnectionState state, const QString &ip, const QString &ssid) {
	mConnectionState = state;
	mCurrentSsid = "";

	if (mConnectionState == ConnectionState::Connected) {
		mCurrentSsid = ssid;
		mIpValue = ip;
	}

	Q_EMIT connectionStateChanged();
	Q_EMIT currentSsidChanged();
}

void WiFiClientMock::connectToSelectedNetwork(QString ssid) {
	if (ssid == mCurrentSsid) {
		return;
	}

	if (!mNetworks.contains(ssid)) {
		return;
	}
	const auto doConnect = [&]() { setConnectionStatus(ConnectionState::Connecting, "", ""); };

	if (mNetworks[ssid].mIsKnown) {
		doConnect();
	} else if (mNetworks[ssid].mSecurity == Security::none) {
		doConnect();
		mNetworks[ssid].mIsKnown = true;
	}
	setConnectionStatus(ConnectionState::Connected, "192.168.0.0", "trik wi-fi");
}

int WiFiClientMock::rowCount(const QModelIndex &parent) const {
	if (parent.isValid()) {
		return -1;
	}
	return mAvailableNetworks.size();
}

QVariant WiFiClientMock::data(const QModelIndex &index, int role) const {
	if (!index.isValid() || role != Qt::DisplayRole) {
		return {};
	}

	const int index_row{static_cast<int>(index.row())};
	return QVariant::fromValue(mAvailableNetworks[index_row]);
}

QVector<trikGui::NetworkInfoMock> WiFiClientMock::availableNetworks() { return mAvailableNetworks; }

WiFiClientMock::ConnectionState WiFiClientMock::connectionState() { return mConnectionState; }

QString WiFiClientMock::currentSsid() { return mCurrentSsid; }

QString WiFiClientMock::ipValue() { return mIpValue; }
