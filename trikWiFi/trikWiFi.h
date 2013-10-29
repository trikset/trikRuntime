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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QtCore/QSocketNotifier>

#include "trikWiFi_global.h"
#include "trikWPACtrlIface.h"

class TRIKWIFISHARED_EXPORT TrikWiFi : public QObject
{
	Q_OBJECT
public:
	TrikWiFi(QString interfaceFilePrefix, QString daemonFile, QObject *parent = NULL);
	~TrikWiFi();
	int connect(int id);
	int disconnect();
	struct ScanResult
	{
		QString ssid;
		int frequency;
	};
	int scan();
	QList<ScanResult> scanResults();
	int addNetwork();
	int removeNetwork(int id);
	int setSsid(int id, QString ssid);
	int setKey(int id, QString key);
	int saveConfiguration();
	struct NetworkConfiguration
	{
		int id;
		QString ssid;
	};
	QList<NetworkConfiguration> listNetworks();

private:
	TrikWPACtrlIface mControlIface, mMonitorIface;
	QScopedPointer<QSocketNotifier> mSocketNotifier;
	void processMessage(const QString &message);

private slots:
	void receiveMessages();

signals:
	void scanFinished();
};
