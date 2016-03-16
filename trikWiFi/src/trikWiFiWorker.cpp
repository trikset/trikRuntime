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

#include <QsLog.h>

#include "wpaSupplicantCommunicator.h"

using namespace trikWiFi;

TrikWiFiWorker::TrikWiFiWorker(const QString &interfaceFilePrefix
		, const QString &daemonFile)
	: mInterfaceFile(interfaceFilePrefix)
	, mDaemonFile(daemonFile)
{
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

	mStatus.reset();
	mNetworkConfiguration.reset();
	mScanResult.reset();

	mMonitorInterface->attach();
	const int monitorFileDesc = mMonitorInterface->fileDescriptor();
	if (monitorFileDesc >= 0) {
		mMonitorFileSocketNotifier.reset(new QSocketNotifier(monitorFileDesc, QSocketNotifier::Read));
		QObject::connect(mMonitorFileSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(receiveMessages()));
	} else {
		QLOG_ERROR() << "Can not get monitor file descriptor";
	}

	statusRequest();
	listNetworksRequest();
	scanRequest();

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

void TrikWiFiWorker::connect(int id)
{
	QString reply;
	int result = mControlInterface->request("DISCONNECT", reply);
	if (result < 0 || reply != "OK\n") {
		emit error("connect");
		return;
	}

	result = mControlInterface->request("SELECT_NETWORK " + QString::number(id), reply);
	if (result < 0 || reply != "OK\n") {
		emit error("connect");
		return;
	}
}

void TrikWiFiWorker::connectToOpenNetwork(const QString &ssid)
{
	// Disconnecting from previous network.
	QString reply;
	mIgnoreDisconnect = true;

	int result = mControlInterface->request("DISCONNECT", reply);
	if (result < 0 || reply != "OK\n") {
		emit error("connect");
		return;
	}

	// At first checking if we already have this network in configuration.
	int networkId = findNetworkId(ssid);

	// If not, adding it.
	if (networkId == -1) {
		networkId = addOpenNetwork(ssid);
	}

	if (networkId == -1) {
		// Connection failed.
		return;
	}

	// Now connect to the selected network.
	result = mControlInterface->request("SELECT_NETWORK " + QString::number(networkId), reply);
	if (result < 0 || reply != "OK\n") {
		emit error("connect");
		return;
	}

	// Refresh network configuration.
	/// @todo: maybe do it manually?
	listNetworksRequest();
}

void TrikWiFiWorker::disconnect()
{
	QString reply;
	const int result = mControlInterface->request("DISCONNECT", reply);
	if (result != 0 || reply != "OK\n") {
		emit error("disconnect");
	}
}

void TrikWiFiWorker::scanRequest()
{
	QString reply;

	qDebug() << "TrikWiFiWorker::scanRequest()";

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

	QHash<QString, QString> parsedReply = parseReply(reply);

	mStatus->connected = parsedReply.contains("ssid") && !parsedReply["ssid"].isEmpty();
	if (mStatus->connected) {
		mStatus->ipAddress = parsedReply["ip_address"];
		mStatus->ssid = parsedReply["ssid"];
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

		qDebug() << "================ Sending:" << command;

		QString reply;

		if (mControlInterface->request(command, reply) < 0) {
			break;
		}

		const QHash<QString, QString> parsedReply = parseReply(reply);

		if (parsedReply.isEmpty()) {
			break;
		}

		ScanResult currentResult;

		if (parsedReply.contains("freq")) {
			currentResult.frequency = parsedReply["freq"].toInt();
		} else {
			currentResult.frequency = 0;
		}

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

		currentResult.networkId = findNetworkId(currentResult.ssid);

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

void TrikWiFiWorker::listNetworksRequest()
{
	QString reply;
	const int result = mControlInterface->request("LIST_NETWORKS", reply);
	if (result < 0 || reply.isEmpty() || reply.startsWith("FAIL")) {
		emit error("listNetworksRequest");
		return;
	}

	const QStringList lines = reply.split('\n');
	for (const QString &line : lines) {
		QStringList const values = line.split('\t');
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
		mNetworkConfiguration->append(currentNetwork);
	}

	mNetworkConfiguration.sync();
	mNetworkConfiguration->clear();
}

QList<NetworkConfiguration> TrikWiFiWorker::listNetworksResult()
{
	return mNetworkConfiguration.get();
}

void TrikWiFiWorker::processMessage(const QString &message)
{
	if (message.contains("CTRL-EVENT-SCAN-RESULTS") && !mIgnoreScanResults) {
		processScanResults();
	} else if (message.contains("CTRL-EVENT-CONNECTED")) {
		emit connected();
	} else if (message.contains("CTRL-EVENT-DISCONNECTED") && !mIgnoreDisconnect) {
		emit disconnected();
	}
}

void TrikWiFiWorker::receiveMessages()
{
	while (mMonitorInterface->isPending()) {
		QString message;
		if (mMonitorInterface->receive(message) == 0) {
			qDebug() << "==================== MESSAGE" << message;
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

	qDebug() << reply;

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

	qDebug() << reply;

	if (reply.startsWith("FAIL")) {
		return -1;
	}

	QString list;

	mControlInterface->request("LIST_NETWORKS", list);

	qDebug() << list;

	bool ok = false;
	const int id = reply.toInt(&ok);
	if (ok) {
		mControlInterface->request("SET_NETWORK " + QString::number(id) + " ssid \"" + ssid + "\"", reply);
		qDebug() << reply;
		if (reply != "OK\n") {
			return -1;
		}
		mControlInterface->request("SET_NETWORK " + QString::number(id) + " key_mgmt NONE", reply);
		qDebug() << reply;
		if (reply != "OK\n") {
			return -1;
		}
		mControlInterface->request("ENABLE_NETWORK " + QString::number(id), reply);
		qDebug() << reply;
		if (reply != "OK\n") {
			return -1;
		}
		return id;
	} else {
		return -1;
	}
}

int TrikWiFiWorker::findNetworkId(const QString &ssid) const
{
	for (const NetworkConfiguration &network : *mNetworkConfiguration) {
		if (network.ssid == ssid) {
			return network.id;
		}
	}

	return -1;
}
