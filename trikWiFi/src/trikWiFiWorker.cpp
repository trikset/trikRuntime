/* Copyright 2015 Yurii Litvinov, CyberTech Labs Ltd.
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

#include "trikWiFiWorker.h"

#include <QtCore/QStringList>
#include <QtCore/QProcess>

#include <QsLog.h>

#include "wpaSupplicantCommunicator.h"

using namespace trikWiFi;

TrikWiFiWorker::TrikWiFiWorker(const QString &interfaceFilePrefix
	, const QString &daemonFile)
	: mInterfaceFile(interfaceFilePrefix)
	, mDaemonFile(daemonFile)
{
	qRegisterMetaType<DisconnectReason>("DisconnectReason");
}

TrikWiFiWorker::~TrikWiFiWorker()
{
	mMonitorInterface->detach();
}

void TrikWiFiWorker::reinit()
{
	QLOG_INFO() << "Reinitializing WiFi";

	if (!mMonitorInterface.isNull()) {
		mMonitorInterface->detach();
	}

	mControlInterface.reset(new WpaSupplicantCommunicator(mInterfaceFile + "ctrl", mDaemonFile));
	mMonitorInterface.reset(new WpaSupplicantCommunicator(mInterfaceFile + "mon", mDaemonFile));

	mNetworkConfiguration.clear();

	mStatus.reset();
	mScanResult.reset();

	mMonitorInterface->attach();
	const int monitorFileDesc = mMonitorInterface->fileDescriptor();
	if (monitorFileDesc >= 0) {
		mMonitorFileSocketNotifier.reset(new QSocketNotifier(monitorFileDesc, QSocketNotifier::Read));
		QObject::connect(mMonitorFileSocketNotifier.data(),
			SIGNAL(activated(int)),
			this,
			SLOT(receiveMessages()));
	} else {
		QLOG_ERROR() << "Can not get monitor file descriptor";
	}

	listKnownNetworks();
	statusRequest();

	QLOG_INFO() << "WiFi initialized";
}

void TrikWiFiWorker::dispose()
{
	if (!mMonitorInterface.isNull()) {
		mMonitorInterface->detach();
	}

	mControlInterface.reset();
	mMonitorInterface.reset();
}

void TrikWiFiWorker::connect(const QString &ssid)
{
	mPlannedDisconnect = true;

	// At first checking if we have this network in configuration.
	int networkId = findNetworkId(ssid);

	// If not, adding it, assuming that it is open wifi network.
	if (networkId == -1) {
		networkId = addOpenNetwork(ssid);
	}

	if (networkId == -1) {
		// wpa_supplicant failed for some reason.
		emit error("connect");
		return;
	}

	QString reply;
	int result = mControlInterface->request("DISCONNECT", reply);
	if (result < 0 || reply != "OK\n") {
		emit error("connect");
		return;
	}

	result = mControlInterface->request("SELECT_NETWORK " + QString::number(networkId), reply);
	if (result < 0 || reply != "OK\n") {
		emit error("connect");
		return;
	}
}

void TrikWiFiWorker::disconnect()
{
	mPlannedDisconnect = true;

	QString reply;
	const int result = mControlInterface->request("DISCONNECT", reply);
	if (result != 0 || reply != "OK\n") {
		emit error("disconnect");
	}
}

void TrikWiFiWorker::scanRequest()
{
	QString reply;

	mIgnoreScanResults = false;
	const int result = mControlInterface->request("SCAN", reply);
	if (result != 0 || reply != "OK\n") {
		emit error("scanRequest");
	}
}

void TrikWiFiWorker::statusRequest()
{
	const QString command = "STATUS";
	QString reply;

	if (mControlInterface->request(command, reply) < 0) {
		emit error("statusRequest");
		return;
	}

	const QHash<QString, QString> parsedReply = parseReply(reply);

	mStatus->connected = parsedReply.contains("ssid") && !parsedReply["ssid"].isEmpty();
	if (mStatus->connected) {
		mStatus->ipAddress = parsedReply["ip_address"];
		mStatus->ssid = parsedReply["ssid"];
	} else {
		mStatus->ipAddress = "";
		mStatus->ssid = "";
	}

	mStatus.sync();
	emit statusReady();
}

Status TrikWiFiWorker::statusResult()
{
	return mStatus.get();
}

void TrikWiFiWorker::processScanResults()
{
	int index = 0;

	mIgnoreScanResults = true;

	forever {
		const QString command = "BSS " + QString::number(index++);
		QString reply;

		if (mControlInterface->request(command, reply) < 0) {
			break;
		}

		const QHash<QString, QString> parsedReply = parseReply(reply);

		if (parsedReply.isEmpty()) {
			break;
		}

		ScanResult currentResult;

		if (parsedReply.contains("ssid")) {
			currentResult.ssid = parsedReply["ssid"];
		}

		if (parsedReply.contains("flags")) {
			if (parsedReply["flags"].contains("[WPA")) {
				currentResult.security = Security::wpa;
			} else if (parsedReply["flags"].contains("[WEP")) {
				currentResult.security = Security::wep;
			} else {
				currentResult.security = Security::none;
			}
		} else {
			currentResult.security = Security::none;
		}

		currentResult.known = findNetworkId(currentResult.ssid) != -1;

		mScanResult->append(currentResult);
	}

	mScanResult.sync();
	mScanResult->clear();

	emit scanFinished();
}

QList<ScanResult> TrikWiFiWorker::scanResult()
{
	return mScanResult.get();
}

void TrikWiFiWorker::listKnownNetworks()
{
	QString reply;
	const int result = mControlInterface->request("LIST_NETWORKS", reply);
	if (result < 0 || reply.isEmpty() || reply.startsWith("FAIL")) {
		emit error("listNetworksRequest");
		return;
	}

	mNetworkConfiguration.clear();

	const QStringList lines = reply.split('\n');
	for (const QString &line : lines) {
		const QStringList values = line.split('\t');
		if (values.size() != 4) {
			continue;
		}

		NetworkConfiguration currentNetwork;
		bool ok = false;
		currentNetwork.id = values[0].toInt(&ok);
		if (!ok) {
			currentNetwork.id = -1;
		}

		currentNetwork.ssid = values[1];
		mNetworkConfiguration.insert(currentNetwork.ssid, currentNetwork);
	}
}

void TrikWiFiWorker::processMessage(const QString &message)
{
	if (message.contains("CTRL-EVENT-SCAN-RESULTS") && !mIgnoreScanResults) {
		processScanResults();
	} else if (message.contains("CTRL-EVENT-CONNECTED")) {
		// Refresh connection status.
		statusRequest();
		mPlannedDisconnect = false;
		// hotfix: here we trying to get new ip for new connection
		QProcess dhcpProcess;
		dhcpProcess.start("udhcpc", {"-i", "wlan0"});
		dhcpProcess.waitForFinished(3000);
		statusRequest();
		emit connected();
	} else if (message.contains("CTRL-EVENT-DISCONNECTED")) {
		emit disconnected(mPlannedDisconnect ? DisconnectReason::planned : DisconnectReason::unplanned);
	}
}

void TrikWiFiWorker::receiveMessages()
{
	while (mMonitorInterface->isPending()) {
		QString message;
		if (mMonitorInterface->receive(message) == 0) {
			processMessage(message);
		}
	}
}

QHash<QString, QString> TrikWiFiWorker::parseReply(const QString &reply)
{
	QHash<QString, QString> result;

	if (reply.isEmpty() || reply.startsWith("FAIL")) {
		return result;
	}

	const QStringList lines = reply.split('\n');

	for (const QString &line : lines) {
		const int valuePos = line.indexOf('=') + 1;
		if (valuePos < 1) {
			continue;
		}

		const QString key = line.left(valuePos - 1);
		const QString value = line.mid(valuePos);

		result.insert(key, value);
	}

	return result;
}

int TrikWiFiWorker::addOpenNetwork(const QString &ssid)
{
	QString reply;

	mControlInterface->request("ADD_NETWORK", reply);
	if (reply.startsWith("FAIL")) {
		return -1;
	}

	bool ok = false;
	const int id = reply.toInt(&ok);
	if (ok) {
		mControlInterface->request(QString("SET_NETWORK %1 ssid \"%2\"").arg(id).arg(ssid), reply);
		if (reply != "OK\n") {
			return -1;
		}

		mControlInterface->request(QString("SET_NETWORK %1 key_mgmt NONE").arg(id), reply);
		if (reply != "OK\n") {
			return -1;
		}

		// Enable all networks before saving config, to avoid accidentally turning off known networks.
		mControlInterface->request("ENABLE_NETWORK all", reply);
		if (reply != "OK\n") {
			return -1;
		}

		mControlInterface->request("SAVE_CONFIG", reply);
		if (reply != "OK\n") {
			return -1;
		}

		mNetworkConfiguration.insert(ssid, {id, ssid});

		return id;
	} else {
		return -1;
	}
}

int TrikWiFiWorker::findNetworkId(const QString &ssid) const
{
	return mNetworkConfiguration.value(ssid, {-1, ""}).id;
}
