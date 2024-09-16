/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QtCore/QSocketNotifier>
#include <QtCore/QHash>

#include <trikKernel/synchronizedVar.h>

#include "networkStructs.h"

namespace trikWiFi {

class WpaSupplicantCommunicator;

/// Worker object for TrikWiFi, supposed to be runned in its own thread.
class TrikWiFiWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param interfaceFilePrefix - path and prefix of file names that are used by this library to communicate
	///        with wpa_supplicant.
	/// @param daemonFile - file that wpa_supplicant uses to communicate with clients, something like
	///        /run/wpa_supplicant/wlan0.
	TrikWiFiWorker(const QString &interfaceFilePrefix, const QString &daemonFile);

	~TrikWiFiWorker() override;

	/// Connect to a network with given ssid.
	Q_INVOKABLE void connect(const QString &ssid);

	/// Disconnect from network if we are currently connected to one.
	Q_INVOKABLE void disconnect();

	/// Disposes an old connection to wpa_supplicant and creates a new one.
	Q_INVOKABLE void reinit();

	/// Closes the connection to wpa_supplicant.
	Q_INVOKABLE void dispose();

	/// Gets conection status and connection information such as SSID and IP. Emits statusReady when
	/// done, then status can be obtained by statusResult() call.
	Q_INVOKABLE void statusRequest();

	/// Returns last known connection status. To refresh, use statusRequest() method.
	Status statusResult();

	/// Scans for available WiFi networks. When done, sends scanFinished() signal, then scan results
	/// can be obtained via scanResult() method.
	Q_INVOKABLE void scanRequest();

	/// Returns a list of currently known available WiFi networks. Use scanRequest() method to refresh.
	QList<ScanResult> scanResult();

signals:
	/// Emitted when scanning for available networks initiated by scan() is finished and results are available
	/// and ready to be obtained by scanResults method.
	void scanFinished();

	/// Emitted when wpa_supplicant connects to WiFi network. SSID of this network can be retrieved by status() method.
	void connected();

	/// Emitted when wpa_supplicant disconnects from current network.
	void disconnected(trikWiFi::DisconnectReason reason);

	/// Emitted when connection status requested by statusRequest() is ready and results can be obtained by
	/// statusResult() method.
	void statusReady();

	/// Emitted when something goes wrong.
	void error(const QString &message);

private slots:
	/// Slot that processes messages from wpa_supplicant.
	void receiveMessages();

private:
	/// Contains configuration entry from wpa-supplicant config.
	struct NetworkConfiguration {
		/// Some unique id of a network.
		int id;

		/// SSID of a network.
		QString ssid;
	};

	/// Parses BSS record into (key, value) parts.
	static QHash<QString, QString> parseReply(const QString &reply);

	/// Dispatcher for wpa_supplicant events.
	void processMessage(const QString &message);

	/// Iterates through BSS records prepared by SCAN command and gets information about available access points.
	void processScanResults();

	/// Adds an open network with given SSID into wpa_supplicant.conf.
	int addOpenNetwork(const QString &ssid);

	/// Returns id of a network in a list of networks with known configuration or -1 if the network is unknown.
	int findNetworkId(const QString &ssid) const;

	/// Gets registered networks from wpa_supplicant.
	void listKnownNetworks();

	QString mInterfaceFile;
	QString mDaemonFile;
	QScopedPointer<WpaSupplicantCommunicator> mControlInterface;
	QScopedPointer<WpaSupplicantCommunicator> mMonitorInterface;
	QScopedPointer<QSocketNotifier> mMonitorFileSocketNotifier;

	/// Networks listed in /etc/wpa_supplicant.conf hashed by SSID.
	QHash<QString, NetworkConfiguration> mNetworkConfiguration;

	trikKernel::SynchronizedVar<Status> mStatus;

	/// Result of the last scan for networks in range.
	trikKernel::SynchronizedVar<QList<ScanResult>> mScanResult;

	/// wpa_supplicant may send several CTRL-EVENT-SCAN-RESULTS messages for one scan request for some reason,
	/// we need only one and then ignore all others until we explicitly request scan again.
	bool mIgnoreScanResults = true;

	/// Flag that is set when we are disconnecting by our own choice and reset when we become connected.
	bool mPlannedDisconnect = false;
};

}
