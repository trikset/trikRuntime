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
 * limitations under the License. */

/* This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#pragma once

#include <QtCore/qglobal.h>

#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <trikWiFi/networkStructs.h>


namespace trikWiFi {
class TrikWiFi;
}

namespace trikGui {
/// Information about networks that we get from WiFi controller when scans are
/// complete.
struct NetworkInfo {
	Q_GADGET
	Q_PROPERTY(QString ssid MEMBER mSsid)
	Q_PROPERTY(bool isKnown MEMBER mIsKnown)
	Q_PROPERTY(QString security READ security)

public:
	QString security() const {
		if (this->mSecurity == trikWiFi::Security::wpa) {
			return "wpa";
		} else if (this->mSecurity == trikWiFi::Security::wep) {
			return "wep";
		} else {
			return "none";
		}
	}
	/// SSID of the network.
	QString mSsid;

	/// Is this network known (listed in /etc/wpa_supplicant.conf), so it
	/// has all configuration parameters required to connect.
	bool mIsKnown;

	/// Type of security algorithm used by this network.
	trikWiFi::Security mSecurity;
};

/// Widget that shows current IP address and a list of available WiFi networks.
/// Network is available when it is listed in /etc/wpa_supplicant.conf or it is open network without security.
///
/// Protocol of communication between this widget and underlying WiFi controller is highly asynchronous due to
/// use of wpa_supplicant, which uses asynchronous message passing to communicate with client software.
/// 1. Upon construction widget requests status from WiFi controller.
/// 2. Upon construction widget requests scan from WiFi controller (so every time this widget is opened it will
///    ask WiFi controller to rescan networks, no "Rescan" button is needed).
/// 3. On status response it shows IP address and connection status.
/// 4. On Scan response it refreshes list of available networks (and shows them instead of "Scanning..." message). It
///    also clears internal network info model and repopulates it with new information.
/// 5. On selecting new network in a network list widget moves to "connecting" state and then tries to connect.
/// 6. After "connect" request widget expects "disconnected" signal and then "connected" signal.
///    6.1. Planned "disconnected" signal is ignored since widget is already in "connecting" state.
///    6.2. "connected" signal moves widget to "connected" state and causes to refresh ip address and connection status
///         indication.
///    6.3. If widget does not receive "connected" signal in time, it will move to "errored" state (with proper
///         indication).
/// 7. If a widget gets unplanned "disconnect" signal from WiFi controller, it will move to "notConnected" state and
///    initiate network scan.
/// 8. Receiving "scanFinished" signal will cause widget to refresh available networks list.
/// 9. It is possible to receive unplanned "connected" signal, widget will update connection information accordingly.
class WiFiClient : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QVector<trikGui::NetworkInfo> availableNetworks READ availableNetworks NOTIFY availableNetworksChanged)
	Q_PROPERTY(ConnectionState connectionState READ connectionState NOTIFY connectionStateChanged)
	Q_PROPERTY(QString currentSsid READ currentSsid NOTIFY currentSsidChanged)
	Q_PROPERTY(QString ipValue READ ipValue NOTIFY ipValueChanged)

public:
	/// Constructor.
	/// @param parent - parent QObject.
	explicit WiFiClient(trikWiFi::TrikWiFi &trikWiFi, QObject *parent = nullptr);

	/// Destructor.
	~WiFiClient() override;

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

public:
	/// Enum with possible states of a widget. It is actually an automata
	/// that can move between four connection states reacting on user input
	/// and WiFi controller messages.
	enum class ConnectionState { NotConnected, Connecting, Connected, Errored };
	Q_ENUM(ConnectionState)
	/// Tries to connect to currently selected network.
	Q_INVOKABLE void connectToSelectedNetwork(QString ssid);

	Q_INVOKABLE void setQmlParent(QObject *parent);
	/// Send requests for scanning
	void scanWiFi();

private Q_SLOTS:
	/// Called when information about networks in range is changed in WiFi
	/// controller.
	void onNetworksInfoUpdated();

	/// Called when we become connected to some network.
	void onConnected();

	/// Called when we become disconnected by some reason.If reason is
	/// "planned" we were aware that we will disconnect and it is ok, if
	/// "unplanned", then disconnection was caused by external reasons.
	void onDisconnected(trikWiFi::DisconnectReason reason);

	/// Called when connection status is changed in WiFi controller.
	void onStatusUpdated();

	/// Called when there is an error in WiFi controller (related to
	/// wpa_supplicant, for example, if it died horribly).
	void onError(const QString &message);

	/// Called when we are trying to connect to a network for too long.
	void onConnectionTimeout();

private:
	/// Updates widget state and shows info about connection in GUI.
	void setConnectionStatus(ConnectionState state, const QString &ip, const QString &ssid);

	QVector<QString> mAvailableNetworksSsids;
	QVector<NetworkInfo> mAvailableNetworks;
	trikWiFi::TrikWiFi &mWiFi;
	QString mCurrentSsid;
	QHash<QString, NetworkInfo> mNetworks;
	ConnectionState mConnectionState;
	QString mIpValue;
	bool isScanning{true};

	QTimer mConnectionTimeoutTimer;
	ConnectionState connectionState();
	QString currentSsid();
	QString ipValue();
	QVector<trikGui::NetworkInfo> availableNetworks();

Q_SIGNALS:
	/// Emitted when available networks changed
	void availableNetworksChanged();
	/// Emitted when connection state changed
	void connectionStateChanged();
	/// Emitted when current ssid changed
	void currentSsidChanged();
	/// Emitted when ip value changed
	void ipValueChanged();
};

}
Q_DECLARE_METATYPE(trikGui::NetworkInfo)