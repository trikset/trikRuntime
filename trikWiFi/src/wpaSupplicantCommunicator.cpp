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

#include "src/wpaSupplicantCommunicator.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <QtCore/QString>

#include "QsLog.h"

using namespace trikWiFi;

WpaSupplicantCommunicator::WpaSupplicantCommunicator(
		const QString &interfaceFile
		, const QString &daemonFile
		, QObject *parent
		)
	: QObject(parent)
	, mLocal(new sockaddr_un())
	, mDest(new sockaddr_un())
{
	mSocket = socket(PF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);

	if (mSocket < 0) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ << "Cannot create a socket:" << strerror(errno);
		return;
	}

	mLocal->sun_family = AF_UNIX;
	snprintf(mLocal->sun_path, sizeof(mLocal->sun_path), "%s", interfaceFile.toStdString().c_str());
	unlink(mLocal->sun_path);
	if (bind(mSocket, static_cast<sockaddr *>(static_cast<void*>(mLocal.data())), sizeof(*(mLocal.data()))) != 0) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ << "Cannot bind a name to a socket:" << strerror(errno);
		close(mSocket);
		mSocket = -1;
		return;
	}

	mDest->sun_family = AF_UNIX;
	snprintf(mDest->sun_path, sizeof(mDest->sun_path), "%s", daemonFile.toStdString().c_str());
	if (::connect(mSocket, static_cast<sockaddr *>(static_cast<void*>(mDest.data())), sizeof(*(mDest.data()))) != 0) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ << "Cannot connect a socket:" << strerror(errno);
		unlink(mLocal->sun_path);
		close(mSocket);
		mSocket = -1;
		return;
	}
}

WpaSupplicantCommunicator::~WpaSupplicantCommunicator()
{
	if (mSocket >= 0) {
		if (close(mSocket) != 0) {
			QLOG_ERROR() << "Cannot close socket:" << strerror(errno);
		}

		if (unlink(mLocal->sun_path) != 0) {
			QLOG_ERROR() << "Cannot unlink:" << strerror(errno);
		}
	}
}

int WpaSupplicantCommunicator::fileDescriptor()
{
	return mSocket;
}

int WpaSupplicantCommunicator::attach()
{
	if (mSocket < 0) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ <<  "Cannot attach, because socket doesn't exist.";
		return -1;
	}

	const QString command = "ATTACH";
	QString reply;
	const int result = request(command, reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

int WpaSupplicantCommunicator::detach()
{
	if (mSocket < 0) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ << "Cannot detach, because socket doesn't exist.";
		return -1;
	}

	QString command = "DETACH";
	QString reply;
	const int result = request(command, reply);
	if (result == 0 && reply == "OK\n") {
		return 0;
	} else {
		return -1;
	}
}

int WpaSupplicantCommunicator::request(const QString &command, QString &reply)
{
	if (mSocket < 0) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ << "Cannot request, because socket doesn't exist.";
		return -1;
	}

	auto const &commandAscii = command.toStdString();
	if (send(mSocket, commandAscii.c_str(), commandAscii.size()+1, 0) < 0) {
		QLOG_ERROR() << __PRETTY_FUNCTION__ << "Cannot send a message to the daemon:" << strerror(errno);
		return -1;
	}

	forever {
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(mSocket, &rfds);
		struct timeval tv {};
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		select(mSocket + 1, &rfds, nullptr, nullptr, &tv);
		if (FD_ISSET(mSocket, &rfds)) {
			const int bufferSize = 2048;
			char buffer[bufferSize];
			auto replyLen = recv(mSocket, buffer, bufferSize, 0);
			if (replyLen < 0) {
				QLOG_ERROR() << "Cannot receive a reply from the daemon:" << strerror(errno);
				return -1;
			} else if (buffer[0] == '<') { //unsolicited message
				continue;
			} else {
				buffer[replyLen] = '\0';
				reply = buffer;
				return 0;
			}
		} else {
			QLOG_ERROR() << "No reply from the daemon";
			return -1;
		}
	}
}

bool WpaSupplicantCommunicator::isPending()
{
	struct timeval tv {};
	fd_set rfds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(mSocket, &rfds);
	select(mSocket + 1, &rfds, nullptr, nullptr, &tv);
	return FD_ISSET(mSocket, &rfds);
}

int WpaSupplicantCommunicator::receive(QString &message)
{
	const int bufferSize = 256;
	char buffer[bufferSize];
	const int messageLen = recv(mSocket, buffer, bufferSize, 0);
	if (messageLen < 0) {
		QLOG_ERROR() << "Cannot receive a message from the daemon:" << strerror(errno);
		return -1;
	}

	buffer[messageLen] = '\0';
	message = buffer;

	return 0;
}
