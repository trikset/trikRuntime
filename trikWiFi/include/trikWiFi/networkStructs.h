/* Copyright 2015 - 2016 Yurii Litvinov and CyberTech Labs Ltd.
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

/// Enum with possible network security
enum class Security
{
	/// Uses some kind of WPA security algorithm. Authorization required.
	wpa

	/// Uses some kind of WEP security algorithm. Authorization may be required.
	, wep

	/// It is an open network.
	, none
};

/// Enum with possible reasons of "disconnect" message.
enum class DisconnectReason
{
	/// Disconnected ourselves.
	planned

	/// Disconnected for some external reason.
	, unplanned
};

enum class SignalStrength
{
	undefined
	, low
	, medium
	, high
};

/// Contains info about current connection.
struct Status
{
	/// True, if there is active WiFi connection. All other fields are meaningless if this field is false.
	bool connected = false;

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

	/// Security algorithm used in that network.
	Security security {};

	/// Is this network known --- listed in /etc/wpa_supplicant.conf.
	bool known {};
};

}

