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

#include <trikKernel/bufferedVar.h>

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

	/// Connect to a network with given id. Available ids can be obtained by listNetworks method.
	Q_INVOKABLE void connect(int id);

	/// Disconnect from network if we are currently connected to one.
	Q_INVOKABLE void disconnect();

	/// Disposes an old connection to wpa_supplicant and creates a new one.
	Q_INVOKABLE void reinit();

	/// Closes the connection to wpa_supplicant.
	Q_INVOKABLE void dispose();

	Q_INVOKABLE void listNetworksRequest();

	Q_INVOKABLE void statusRequest();

	Q_INVOKABLE void scanRequest();

	QList<NetworkConfiguration> listNetworksResult() const;

	/// Returns last known connection status. To refresh, use statusRequest() method.
	Status statusResult() const;

	QList<ScanResult> scanResult() const;

signals:
	/// Emitted when scanning for available networks initiated by scan() is finished and results are available
	/// and ready to be obtained by scanResults method.
	void scanFinished();

	/// Emitted when wpa_supplicant connects to WiFi network. SSID of this network can be retrieved by status() method.
	void connected();

	void error(const QString &message);

	/// Emitted when wpa_supplicant disconnects from current network.
	void disconnected();

	void statusReady();

	void listNetworksReady();

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

	void processScanResults();

	trikKernel::BufferedVar<Status> mStatus;
//	QScopedPointer<Status> mStatus;
//	QScopedPointer<Status> mStatusBuffer;

	trikKernel::BufferedVar<QList<NetworkConfiguration>> mNetworkConfiguration;
//	QScopedPointer<QList<NetworkConfiguration>> mNetworksList;
//	QScopedPointer<QList<NetworkConfiguration>> mNetworksListBuffer;

	trikKernel::BufferedVar<QList<ScanResult>> mScanResult;
//	QScopedPointer<QList<ScanResult>> mScanResult;
//	QScopedPointer<QList<ScanResult>> mScanResultBuffer;
};

}
