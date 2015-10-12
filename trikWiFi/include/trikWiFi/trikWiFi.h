/* Copyright 2013 Roman Kurbatov, Yurii Litvinov
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

#include "declSpec.h"

namespace trikWiFi {

class WpaSupplicantCommunicator;

/// Contains info about current connection.
struct Status
{
	/// True, if there is active WiFi connection. All other fields are meaningless if this field is false.
	bool connected;

	/// SSID of currently connected network.
	QString ssid;

	/// Current IP address.
	QString ipAddress;
};

/// Contains description of a network obtained by scanning.
struct ScanResult
{
	/// SSID of a network.
	QString ssid;

	/// Channel of a network.
	int frequency;
};

/// Contains configuration entry from wpa-supplicant config.
struct NetworkConfiguration
{
	/// Some unique id of a network.
	int id;

	/// SSID of a network.
	QString ssid;
};

/// Class for WiFi management. Allows to connect and disconnect to/from WPA-PSK network (using wpa-supplicant utility),
/// to scan for available networks and to alter configuration of wpa-supplicant.
class TRIKWIFI_EXPORT TrikWiFi : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param interfaceFilePrefix - path and prefix of file names that are used by this library to communicate
	///        with wpa_supplicant.
	/// @param daemonFile - file that wpa_supplicant uses to communicate with clients, something like
	///        /run/wpa_supplicant/wlan0.
	/// @param parent - parent QObject.
	TrikWiFi(const QString &interfaceFilePrefix, const QString &daemonFile, QObject *parent = nullptr);

	/// Destructor.
	~TrikWiFi() override;

	/// Connect to a network with given id. Available ids can be obtained by listNetworks method.
	int connect(int id);

	/// Disconnect from network if we are currently connected to one.
	int disconnect();

	/// Asynchronously scans for available WiFi networks. When done, sends scanFinished signal, then scan results
	/// can be obtained via scanResults method.
	int scan();

	/// Returns current connection status.
	Status status() const;

	/// Returns results of the last scan.
	QList<ScanResult> scanResults();

	/// Adds new network into wpa_supplicant configuration.
	/// @returns id of a newly added network.
	int addNetwork();

	/// Removes network with given id from wpa_supplicant configuration.
	int removeNetwork(int id);

	/// Sets SSID for a network with given id in wpa_supplicant configuration.
	int setSsid(int id, const QString &ssid);

	/// Sets PSK key for a network with given id in wpa_supplicant configuration.
	int setKey(int id, const QString &key);

	/// Sets key management protocol to "none" for a network with given id in wpa_supplicant configuration.
	int setNoKeyNeeded(int id);

	/// Save current wpa_supplicant configuration in wpa_supplicant config file.
	int saveConfiguration();

	/// Returns a list of networks from wpa_supplicant config.
	QList<NetworkConfiguration> listNetworks();

	/// Disposes an old connection to wpa_supplicant and creates a new one.
	void reinit();

	/// Closes the connection to wpa_supplicant.
	void dispose();

signals:
	/// Emitted when scanning for available networks initiated by scan() is finished and results are available
	/// and ready to be obtained by scanResults method.
	void scanFinished();

	/// Emitted when wpa_supplicant connects to WiFi network. SSID of this network can be retrieved by status() method.
	void connected();

	/// Emitted when wpa_supplicant disconnects from current network.
	void disconnected();

private slots:
	void receiveMessages();

private:
	QString mInterfaceFile;
	QString mDaemonFile;
	QScopedPointer<WpaSupplicantCommunicator> mControlInterface;
	QScopedPointer<WpaSupplicantCommunicator> mMonitorInterface;
	QScopedPointer<QSocketNotifier> mMonitorFileSocketNotifier;

	static QHash<QString, QString> parseReply(const QString &reply);

	void processMessage(const QString &message);
};

}
