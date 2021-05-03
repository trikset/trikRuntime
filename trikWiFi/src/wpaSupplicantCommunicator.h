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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include "trikWifiDeclSpec.h"

struct sockaddr_un;

namespace trikWiFi {

/// Allows communication with wpa_supplicant daemon.
class TRIKWIFI_EXPORT WpaSupplicantCommunicator : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param interfaceFile - file that is used by this library to communicate with wpa_supplicant.
	/// @param daemonFile - file that wpa_supplicant uses to communicate with clients, something like
	///        /run/wpa_supplicant/wlan0.
	/// @param parent - parent QObject.
	WpaSupplicantCommunicator(const QString &interfaceFile, const QString &daemonFile, QObject *parent = nullptr);

	/// Destructor.
	~WpaSupplicantCommunicator();

	/// Handle of a file
	int fileDescriptor();

	/// Attach communicator to wpa_supplicant to be able to receive network events.
	int attach();

	/// Detach communicator from wpa_supplicant.
	int detach();

	/// Send a request to wpa_supplicant.
	/// @param command - command to wpa_supplicant. List of available commands is here:
	///        http://hostap.epitest.fi/wpa_supplicant/devel/ctrl_iface_page.html
	/// @param reply - reply from wpa_supplicant.
	int request(const QString &command, QString &reply);

	/// Returns true if there are unread incoming messages from wpa_supplicant.
	bool isPending();

	/// Receive next incoming message from wpa_supplicant.
	int receive(QString &message);

private:
	int mSocket;
	QScopedPointer<sockaddr_un> mLocal;
	QScopedPointer<sockaddr_un> mDest;
};

}
