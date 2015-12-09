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

#include <QtCore/QString>

namespace trikWiFi {

/// Contains info about current connection.
struct Status
{
	/// True, if there is active WiFi connection. All other fields are meaningless if this field is false.
	bool connected;

	/// SSID of currently connected network.
	QString ssid;

	/// Current IP address.
	QString ipAddress;
};

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

}

