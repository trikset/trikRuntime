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
}

Status TrikWiFiWorker::statusResult()
{
	return mStatus.get();
}

void TrikWiFiWorker::processScanResults()
{
	int index = 0;

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

		// TODO: Add error processing.
		currentResult.frequency = parsedReply["freq"].toInt();
		currentResult.ssid = parsedReply["ssid"];

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
	if (message.contains("CTRL-EVENT-SCAN-RESULTS")) {
		processScanResults();
	} else if (message.contains("CTRL-EVENT-CONNECTED")) {
		emit connected();
	} else if (message.contains("CTRL-EVENT-DISCONNECTED")) {
		emit disconnected();
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
