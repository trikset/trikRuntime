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

#include "trikWiFi.h"

#include <QtCore/QStringList>
#include <QtNetwork/QNetworkInterface>
#include <QtCore/QDebug>

#include <QsLog.h>

#include "wpaSupplicantCommunicator.h"
#include "QsLog.h"

using namespace trikWiFi;

TrikWiFi::TrikWiFi(const QString &interfaceFilePrefix
		, const QString &daemonFile
		, QObject *parent)
	: QObject(parent)
	, mInterfaceFile(interfaceFilePrefix)
	, mDaemonFile(daemonFile)
{
	reinit();
}

TrikWiFi::~TrikWiFi()
{
	mMonitorInterface->detach();
}

void TrikWiFi::reinit()
{
	QLOG_INFO() << "Reinitializing WiFi";

	if (!mMonitorInterface.isNull()) {
		mMonitorInterface->detach();
	}

	mControlInterface.reset(new WpaSupplicantCommunicator(mInterfaceFile + "ctrl", mDaemonFile));
	mMonitorInterface.reset(new WpaSupplicantCommunicator(mInterfaceFile + "mon", mDaemonFile));

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

void TrikWiFi::dispose()
{
	if (!mMonitorInterface.isNull()) {
		mMonitorInterface->detach();
	}

	mControlInterface.reset(nullptr);
	mMonitorInterface.reset(nullptr);
}

int TrikWiFi::connect(int id)
{
	QString reply;
	int result = mControlInterface->request("DISCONNECT", reply);
	if (result < 0 || reply != "OK\n") {
		return -1;
	}

	result = mControlInterface->request("SELECT_NETWORK " + QString::number(id), reply);
	if (result < 0 || reply != "OK\n") {
		return -1;
	}

	return 0;
}

int TrikWiFi::disconnect()
{
	QString reply;
	const int result = mControlInterface->request("DISCONNECT", reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

int TrikWiFi::scan()
{
	QString reply;

	const int result = mControlInterface->request("SCAN", reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

Status TrikWiFi::status() const
{
	const QString command = "STATUS";
	QString reply;

	Status result;

	if (mControlInterface->request(command, reply) < 0) {
		return result;
	}

	QHash<QString, QString> parsedReply = parseReply(reply);

	result.connected = parsedReply.contains("ssid") && !parsedReply["ssid"].isEmpty();
	if (result.connected) {
		result.ipAddress = parsedReply["ip_address"];
		result.ssid = parsedReply["ssid"];
	}

	return result;
}

QList<ScanResult> TrikWiFi::scanResults()
{
	int index = 0;
	QList<ScanResult> results;

	forever {
		const QString command = "BSS " + QString::number(index++);

		QString reply;

		if (mControlInterface->request(command, reply) < 0) {
			break;
		}

		QHash<QString, QString> parsedReply = parseReply(reply);

		if (parsedReply.isEmpty()) {
			break;
		}

		ScanResult currentResult;

		// TODO: Add error processing.
		currentResult.frequency = parsedReply["freq"].toInt();
		currentResult.ssid = parsedReply["ssid"];

		results.append(currentResult);
	}

	return results;
}

int TrikWiFi::addNetwork()
{
	QString reply;
	mControlInterface->request("ADD_NETWORK", reply);
	if (reply.startsWith("FAIL")) {
		return -1;
	}

	bool ok = false;
	const int id = reply.toInt(&ok);
	if (ok) {
		mControlInterface->request("SET_NETWORK " + QString::number(id) + " disabled 1", reply);
		return id;
	} else {
		return -1;
	}
}

int TrikWiFi::removeNetwork(int id)
{
	QString reply;
	const int result = mControlInterface->request("REMOVE_NETWORK " + QString::number(id), reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

int TrikWiFi::setSsid(int id, const QString &ssid)
{
	QString reply;
	const int result
			= mControlInterface->request("SET_NETWORK " + QString::number(id) + " ssid \"" + ssid + "\"", reply);

	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

int TrikWiFi::setKey(int id, const QString &key)
{
	QString reply;
	const int result = mControlInterface->request("SET_NETWORK " + QString::number(id) + " psk \"" + key + "\"", reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

int TrikWiFi::setNoKeyNeeded(int id)
{
	QString reply;
	const int result = mControlInterface->request("SET_NETWORK " + QString::number(id) + " key_mgmt NONE", reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

int TrikWiFi::saveConfiguration()
{
	QString reply;
	const int result = mControlInterface->request("SAVE_CONFIG", reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

QList<NetworkConfiguration> TrikWiFi::listNetworks()
{
	QString reply;
	const int result = mControlInterface->request("LIST_NETWORKS", reply);
	if (result < 0 || reply.isEmpty() || reply.startsWith("FAIL")) {
		return QList<NetworkConfiguration>();
	}

	QStringList const lines = reply.split('\n');
	QList<NetworkConfiguration> list;
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
		list.append(currentNetwork);
	}

	return list;
}

void TrikWiFi::processMessage(const QString &message)
{
	if (message.contains("CTRL-EVENT-SCAN-RESULTS")) {
		emit scanFinished();
	} else if (message.contains("CTRL-EVENT-CONNECTED")) {
		int result = system("udhcpc -i wlan0");
		if (result == 0) {
			emit connected();
		}
	} else if (message.contains("CTRL-EVENT-DISCONNECTED")) {
		emit disconnected();
	}
}

void TrikWiFi::receiveMessages()
{
	while (mMonitorInterface->isPending()) {
		QString message;
		mMonitorInterface->receive(message);
		processMessage(message);
	}
}

QHash<QString, QString> TrikWiFi::parseReply(const QString &reply)
{
	QHash<QString, QString> result;

	if (reply.isEmpty() || reply.startsWith("FAIL")) {
		return result;
	}

	QStringList const lines = reply.split('\n');

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
