/*
 *  Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 */

#include "trikWiFi.h"

#include <QtCore/QStringList>
#include <QtCore/QRegExp>
#include <QtCore/QDebug>

#include "trikWPACtrlIface.h"

TrikWiFi::TrikWiFi(QString interfaceFilePrefix, QString daemonFile, QObject *parent):
	QObject(parent)
	, mControlIface(interfaceFilePrefix + "ctrl", daemonFile)
	, mMonitorIface(interfaceFilePrefix + "mon", daemonFile)
{
	mMonitorIface.attach();
	int monitorFileDesc = mMonitorIface.fileDescriptor();
	if (monitorFileDesc >= 0)
		mSocketNotifier.reset(new QSocketNotifier(monitorFileDesc, QSocketNotifier::Read));
	QObject::connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(receiveMessages()));
}

TrikWiFi::~TrikWiFi()
{
	mMonitorIface.detach();
	mSocketNotifier.reset(NULL);
}

int TrikWiFi::connect(int id)
{
	QString reply;
	int res = 0;
	res = mControlIface.request("DISCONNECT", reply);
	if (res < 0 || reply != "OK\n")
		return -1;
	res = mControlIface.request("SELECT_NETWORK " + QString::number(id), reply);
	if (res < 0 || reply != "OK\n")
		return -1;
	return 0;
}

int TrikWiFi::disconnect()
{
	QString reply;
	int res = mControlIface.request("DISCONNECT", reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

int TrikWiFi::scan()
{
	QString command = "SCAN";
	QString reply;
	int res = mControlIface.request(command, reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

QList<TrikWiFi::ScanResult> TrikWiFi::scanResults()
{
	QString command;
	QString reply;
	int index = 0;
	QList<TrikWiFi::ScanResult> results;
	forever
	{
		command = "BSS " + QString::number(index++);
		if (mControlIface.request(command, reply) < 0)
			break;
		if (reply.isEmpty() || reply.startsWith("FAIL"))
			break;
		TrikWiFi::ScanResult currentResult;
		currentResult.frequency = -1;
		QStringList lines = reply.split(QRegExp("\\n"));
		for (QStringList::const_iterator i = lines.constBegin(); i < lines.constEnd(); ++i)
		{
			int valuePos = (*i).indexOf('=') + 1;
			if (valuePos < 1)
				continue;
			if ((*i).startsWith("ssid="))
				currentResult.ssid = (*i).mid(valuePos);
			else if ((*i).startsWith("freq="))
				currentResult.frequency = (*i).mid(valuePos).toInt();
		}
		results.append(currentResult);
	}
	return results;
}

int TrikWiFi::addNetwork()
{
	QString reply;
	mControlIface.request("ADD_NETWORK", reply);
	if (reply.startsWith("FAIL"))
		return -1;
	bool ok = false;
	int id = reply.toInt(&ok);
	if (ok)
	{
		mControlIface.request("SET_NETWORK " + QString::number(id) + " disabled 1", reply);
		return id;
	}
	else
		return -1;
}

int TrikWiFi::removeNetwork(int id)
{
	QString reply;
	int res = mControlIface.request("REMOVE_NETWORK " + QString::number(id), reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

int TrikWiFi::setSsid(int id, QString ssid)
{
	QString reply;
	int res = mControlIface.request("SET_NETWORK " + QString::number(id) + " ssid \"" + ssid + "\"", reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

int TrikWiFi::setKey(int id, QString key)
{
	QString reply;
	int res = mControlIface.request("SET_NETWORK " + QString::number(id) + " psk \"" + key + "\"", reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

int TrikWiFi::saveConfiguration()
{
	QString reply;
	int res = mControlIface.request("SAVE_CONFIG", reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

QList<TrikWiFi::NetworkConfiguration> TrikWiFi::listNetworks()
{
	QString reply;
	int res = mControlIface.request("LIST_NETWORKS", reply);
	if (res < 0 || reply.isEmpty() || reply.startsWith("FAIL"))
		return QList<NetworkConfiguration>();
	QStringList lines = reply.split(QRegExp("\\n"));
	QList<NetworkConfiguration> list;
	for (QStringList::const_iterator i = lines.constBegin() + 1; i < lines.constEnd(); ++i)
	{
		QStringList values = (*i).split(QRegExp("\\t"));
		if (values.size() != 4)
			continue;
		NetworkConfiguration currentNetwork;
		bool ok = false;
		currentNetwork.id = values[0].toInt(&ok);
		if (!ok)
			currentNetwork.id = -1;
		currentNetwork.ssid = values[1];
		list.append(currentNetwork);
	}
	return list;
}

void TrikWiFi::processMessage(const QString &message)
{
	if (message.contains("CTRL-EVENT-SCAN-RESULTS"))
		emit scanFinished();
}

void TrikWiFi::receiveMessages()
{
	while (mMonitorIface.isPending())
	{
		QString message;
		mMonitorIface.receive(message);
		processMessage(message);
	}
}
