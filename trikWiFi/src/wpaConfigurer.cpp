/* Copyright 2013 Yurii Litvinov
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

#include "wpaConfigurer.h"

#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QDebug>

#include "trikWiFi.h"

using namespace trikWiFi;

struct trikWiFi::NetworkInfo {
	QString ssid;
	QString password;
	bool requirePassword;
};

void WpaConfigurer::configureWpaSupplicant(QString const &configFile, TrikWiFi &wiFi)
{
	QDomDocument wpaConfig("WPAConfig");

	QFile file(configFile);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open " << configFile << "for reading";
		throw "Failed to open WPA config file for reading";
	} if (!wpaConfig.setContent(&file)) {
		file.close();
		qDebug() << configFile << "parsing failed";
		throw "WPA config file parsing failed";
	}

	file.close();

	QList<NetworkInfo*> networkList;

	QDomElement const root = wpaConfig.documentElement();
	QDomNodeList const networks =  root.elementsByTagName("network");
	int const networksCount = networks.length();
	for (int i = 0; i < networksCount; ++i) {
		QDomElement network = networks.at(i).toElement();

		NetworkInfo * const networkInfo = new NetworkInfo();
		networkInfo->ssid = network.attribute("ssid");
		networkInfo->requirePassword = network.attribute("noPassword", "false") != "true";
		networkInfo->password = network.attribute("password", "");
		networkList << networkInfo;
	}

	mergeNetworkConfigurations(networkList, wiFi);

	qDeleteAll(networkList);
}

void WpaConfigurer::mergeNetworkConfigurations(QList<NetworkInfo*> const &networksFromConfig, TrikWiFi &wiFi)
{
	QList<NetworkConfiguration> const networksFromWpaSupplicant = wiFi.listNetworks();

	QHash<QString, int> networksFromWpaSupplicantHash;
	foreach (NetworkConfiguration const &networkConfiguration, networksFromWpaSupplicant) {
		networksFromWpaSupplicantHash.insert(networkConfiguration.ssid, networkConfiguration.id);
	}

	foreach (NetworkInfo const * const networkInfo, networksFromConfig) {
		int const id = networksFromWpaSupplicantHash.contains(networkInfo->ssid)
				? networksFromWpaSupplicantHash[networkInfo->ssid]
				: wiFi.addNetwork();

		wiFi.setSsid(id, networkInfo->ssid);
		if (networkInfo->requirePassword) {
			wiFi.setKey(id, networkInfo->password);
		} else {
			wiFi.setNoKeyNeeded(id);
		}
	}

	wiFi.saveConfiguration();
}
