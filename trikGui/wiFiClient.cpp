/* Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#include "wiFiClient.h"

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include <trikWiFi/trikWiFi.h>
#include <QtQml>
#include <QsLog.h>

using namespace trikGui;
using namespace trikWiFi;

static const int connectionTimeout = 5000;

WiFiClient::WiFiClient(TrikWiFi &trikWiFi, QObject *parent)
	: QAbstractListModel(parent), mWiFi(trikWiFi), mConnectionState(ConnectionState::NotConnected) {
	qmlRegisterUncreatableType<WiFiClient>("WiFiClient", 1, 0, "ConnectionState", "Enum is not a type");
	connect(&mWiFi, &TrikWiFi::scanFinished, this, &WiFiClient::onNetworksInfoUpdated);
	connect(&mWiFi, &TrikWiFi::connected, this, &WiFiClient::onConnected);
	connect(&mWiFi, &TrikWiFi::disconnected, this, &WiFiClient::onDisconnected);
	connect(&mWiFi, &TrikWiFi::statusReady, this, &WiFiClient::onStatusUpdated);
	connect(&mWiFi, &TrikWiFi::error, this, &WiFiClient::onError);

	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();

	mConnectionTimeoutTimer.setInterval(connectionTimeout);
	mConnectionTimeoutTimer.setSingleShot(true);

	connect(&mConnectionTimeoutTimer, &QTimer::timeout, this, &WiFiClient::onConnectionTimeout);
}

WiFiClient::~WiFiClient() {}

void WiFiClient::setQmlParent(QObject *parent) { setParent(parent); }

void WiFiClient::scanWiFi() {
	mWiFi.statusRequest();
	mWiFi.scanRequest();
}

void WiFiClient::onNetworksInfoUpdated() {
	Q_EMIT beginResetModel();
	mAvailableNetworks.clear();

	mNetworks.clear();
	for (const ScanResult &result : mWiFi.scanResult()) {
		NetworkInfo network{result.ssid, result.known, result.security};
		/// If two networks have the same ssid, only last one will be
		/// shown. Some routers boadcast ssids on different channels
		/// and they will be shown as different networks if we do not
		/// filter them here. It is perceived as a bug by users, so we
		/// sacrifice correctness in sake of simplicity.
		mNetworks.insert(network.mSsid, network);
		if (network.mSsid == mCurrentSsid && network.mIsKnown == true) {
			mAvailableNetworks.prepend(network);
		} else {
			mAvailableNetworks.append(network);
		}
	}

	// Q_EMIT availableNetworksChanged();

	isScanning = false;
	Q_EMIT endResetModel();
}

void WiFiClient::onConnected() {
	mConnectionTimeoutTimer.stop();
	const trikWiFi::Status connectionStatus = mWiFi.statusResult();
	setConnectionStatus(ConnectionState::Connected, connectionStatus.ipAddress, connectionStatus.ssid);
}

void WiFiClient::onDisconnected(DisconnectReason reason) {

	if (reason == DisconnectReason::unplanned) {
		// Maybe disconnect was caused by network going out of range, so
		// we need to rescan networks to determine what networks are
		// left.
		mWiFi.scanRequest();
	}

	// Anyway, we are disconnected, so if we are already connecting, we will
	// probably be connected sometime, in other cases we shall report
	// disconnect.
	setConnectionStatus(
		mConnectionState == ConnectionState::Connecting ? mConnectionState : ConnectionState::NotConnected, "", "");
}

void WiFiClient::onStatusUpdated() {
	const trikWiFi::Status connectionStatus = mWiFi.statusResult();
	setConnectionStatus(connectionStatus.connected ? ConnectionState::Connected : ConnectionState::NotConnected,
				connectionStatus.ipAddress, connectionStatus.ssid);
}

void WiFiClient::onError(const QString &message) {
	QLOG_ERROR() << message;
	if (message == "statusRequest") {
		setConnectionStatus(ConnectionState::Errored, "", "");
	}
}

void WiFiClient::onConnectionTimeout() { setConnectionStatus(ConnectionState::Errored, "", ""); }

void WiFiClient::setConnectionStatus(ConnectionState state, const QString &ip, const QString &ssid) {
	mConnectionState = state;
	mCurrentSsid = "";

	if (mConnectionState == ConnectionState::Connected) {
		mCurrentSsid = ssid;
		mIpValue = ip;
	}
	Q_EMIT connectionStateChanged();
	Q_EMIT currentSsidChanged();
}

void WiFiClient::connectToSelectedNetwork(QString ssid) {
	if (ssid == mCurrentSsid) {
		return;
	}

	if (!mNetworks.contains(ssid)) {
		return;
	}
	const auto doConnect = [&]() {
		setConnectionStatus(ConnectionState::Connecting, "", "");
		mWiFi.connect(ssid);
		mConnectionTimeoutTimer.start();
	};

	if (mNetworks[ssid].mIsKnown) {
		doConnect();
	} else if (mNetworks[ssid].mSecurity == Security::none) {
		doConnect();
		mNetworks[ssid].mIsKnown = true;
	}
}

int WiFiClient::rowCount(const QModelIndex &parent) const {
	if (parent.isValid()) {
		return -1;
	}
	return mAvailableNetworks.size();
}

QVariant WiFiClient::data(const QModelIndex &index, int role) const {
	if (!index.isValid() || role != Qt::DisplayRole) {
		return {};
	}

	const int index_row{static_cast<int>(index.row())};
	return QVariant::fromValue(mAvailableNetworks[index_row]);
}

QVector<trikGui::NetworkInfo> WiFiClient::availableNetworks() { return mAvailableNetworks; }

WiFiClient::ConnectionState WiFiClient::connectionState() { return mConnectionState; }

QString WiFiClient::currentSsid() { return mCurrentSsid; }

QString WiFiClient::ipValue() { return mIpValue; }
