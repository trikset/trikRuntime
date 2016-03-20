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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
	#include <QtGui/QTextEdit>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QListView>
#else
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QTextEdit>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListView>
#endif

#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>

#include <trikWiFi/networkStructs.h>

#include "trikGuiDialog.h"

namespace trikWiFi {
class TrikWiFi;
}

namespace trikGui {

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
class WiFiClientWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent QObject.
	explicit WiFiClientWidget(trikWiFi::TrikWiFi &trikWiFi, QWidget *parent = nullptr);

	/// Destructor.
	~WiFiClientWidget();

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	/// Called when information about networks in range is changed in WiFi controller.
	void onNetworksInfoUpdated();

	/// Called when we become connected to some network.
	void onConnected();

	/// Called when we become disconnected by some reason.If reason is "planned" we were aware that we will disconnect
	/// and it is ok, if "unplanned", then disconnection was caused by external reasons.
	void onDisconnected(trikWiFi::DisconnectReason reason);

	/// Called when connection status is changed in WiFi controller.
	void onStatusUpdated();

	/// Called when there is an error in WiFi controller (related to wpa_supplicant, for example, if it died horribly).
	void onError(const QString &message);

	/// Called when we are trying to connect to a network for too long.
	void onConnectionTimeout();

private:
	/// Enum with possible states of a widget. It is actually an automata that can move between four connection states
	/// reacting on user input and WiFi controller messages.
	enum class ConnectionState {
		notConnected
		, connecting
		, connected
		, errored
	};

	/// Information about networks that we get from WiFi controller when scans are complete.
	struct NetworkInfo {
		/// SSID of the network.
		QString ssid;

		/// Is this network known (listed in /etc/wpa_supplicant.conf), so it has all configuration parameters
		/// required to connect.
		bool isKnown;

		/// Type of security algorithm used by this network.
		trikWiFi::Security security;
	};

	/// Updates widget state and shows info about connection in GUI.
	void setConnectionStatus(ConnectionState state, const QString &ip, const QString &ssid);

	/// Updates networks list with current connection status information (highlights network to which we are
	/// connected, sets appropriate icons for network types).
	void updateConnectionStatusesInNetworkList();

	/// Tries to connect to currently selected network.
	void connectToSelectedNetwork();

	/// Cleans networks list and shows "Scanning..." instead.
	void showScanning();

	QLabel mConnectionIconLabel;
	QLabel mIpLabel;
	QLabel mIpValueLabel;
	QLabel mNameLabel;
	QLabel mNameValueLabel;
	QLabel mAvailableNetworksLabel;
	QListView mAvailableNetworksView;
	QStandardItemModel mAvailableNetworksModel;
	QVBoxLayout mMainLayout;
	QHBoxLayout mIpAddressLayout;
	QHBoxLayout mNameLayout;
	trikWiFi::TrikWiFi &mWiFi;
	QString mCurrentSsid;
	QHash<QString, NetworkInfo> mNetworks;
	ConnectionState mConnectionState;

	QTimer mConnectionTimeoutTimer;
};

}
