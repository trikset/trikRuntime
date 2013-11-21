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

#pragma once

#include <QtCore/QString>
#include <QtCore/QList>

#include "declSpec.h"

namespace trikWiFi {

class TrikWiFi;
struct NetworkInfo;

/// Class that implements working with separate WPA configuration file. It reads a config and
/// adds networks from it to wpa_supplicant configuration. If they already exist there, their properties
/// are overwritten.
///
/// Configuration file is an XML with this format:
/// <WPAConfig>
///   <network ssid="[network ssid]" (password="[PSK password]" | noPassword="true") />
/// </WPAConfig>
class WpaConfigurer
{
public:
	/// Reads config file and adds networks from it into wpa_supplicant config using TrikWiFi object.
	/// @param configFile - path to a configuration file.
	/// @param wiFi - reference to WiFi manager.
	static void configureWpaSupplicant(QString const &configFile, TrikWiFi &wiFi);

private:
	static void mergeNetworkConfigurations(QList<NetworkInfo*> const &networksFromConfig, TrikWiFi &wiFi);
};

}
