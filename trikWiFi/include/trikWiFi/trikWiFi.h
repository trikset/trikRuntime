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

#include "declSpec.h"

namespace trikWiFi {

class WpaSupplicantCommunicator;

/// Class for WiFi control. Allows to connect and disconnect to/from WPA-PSK network (using wpa-supplicant utility),
/// to scan for available networks and to alter configuration of wpa-supplicant.
class TRIKWIFI_EXPORT TrikWiFi : public QObject
{
	Q_OBJECT

public:
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

	/// Constructor.
	/// @param interfaceFilePrefix - path and prefix of file names that are used by this library to communicate
	///        with wpa_supplicant.
	/// @param daemonFile - file that wpa_supplicant uses to communicate with clients, something like
	///        /run/wpa_supplicant/wlan0.
	/// @param parent - parent QObject.
	TrikWiFi(QString const &interfaceFilePrefix, QString const &daemonFile, QObject *parent = NULL);

	/// Destructor.
	~TrikWiFi();

	/// Connect to a network with given id. Available ids can be obtained by listNetworks method.
	int connect(int id);

	/// Disconnect from network if we are currently connected to one.
	int disconnect();

	/// Asynchronously scans for available WiFi networks. When done, sends scanFinished signal, then scan results
	/// can be obtained via scanResults method.
	int scan();

	/// Returns results of the last scan.
	QList<ScanResult> scanResults();

	/// Adds new network into wpa_supplicant configuration.
	/// @returns id of a newly added network.
	int addNetwork();

	/// Removes network with given id from wpa_supplicant configuration.
	int removeNetwork(int id);

	/// Sets SSID for a network with given id in wpa_supplicant configuration.
	int setSsid(int id, QString const &ssid);

	/// Sets PSK key for a network with given id in wpa_supplicant configuration.
	int setKey(int id, QString const &key);

	/// Save current wpa_supplicant configuration in wpa_supplicant config file.
	int saveConfiguration();

	/// Returns a list of networks from wpa_supplicant config.
	QList<NetworkConfiguration> listNetworks();

signals:
	/// Emitted when scanning for available networks initiated by scan() is finished and results are available
	/// and ready to be obtained by scanResults method.
	void scanFinished();

private slots:
	void receiveMessages();

private:
	QScopedPointer<WpaSupplicantCommunicator> mControlInterface;
	QScopedPointer<WpaSupplicantCommunicator> mMonitorInterface;
	QScopedPointer<QSocketNotifier> mMonitorFileSocketNotifier;

	void processMessage(const QString &message);
};

}
