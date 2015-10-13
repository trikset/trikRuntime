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
#include <QtCore/QDebug>

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
	mSocket = socket(PF_UNIX, SOCK_DGRAM, 0);

	if (mSocket < 0) {
		std::cerr << "Cannot create a socket:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return;
	}

	mLocal->sun_family = AF_UNIX;
	snprintf(mLocal->sun_path, sizeof(mLocal->sun_path), "%s", interfaceFile.toStdString().c_str());
	unlink(mLocal->sun_path);
	if (bind(mSocket, reinterpret_cast<sockaddr *>(mLocal.data()), sizeof(*(mLocal.data()))) != 0) {
		std::cerr << "Cannot bind a name to a socket:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		close(mSocket);
		mSocket = -1;
		return;
	}

	mDest->sun_family = AF_UNIX;
	snprintf(mDest->sun_path, sizeof(mDest->sun_path), "%s", daemonFile.toStdString().c_str());
	if (::connect(mSocket, reinterpret_cast<sockaddr *>(mDest.data()), sizeof(*(mDest.data()))) != 0) {
		std::cerr << "Cannot connect a socket:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		unlink(mLocal->sun_path);
		close(mSocket);
		mSocket = -1;
		return;
	}

	int flags = fcntl(mSocket, F_GETFL);
	if (flags >= 0) {
		flags |= O_NONBLOCK;
		fcntl(mSocket, F_SETFL, flags);
	}
}

WpaSupplicantCommunicator::~WpaSupplicantCommunicator()
{
	if (mSocket >= 0) {
		if (close(mSocket) != 0) {
			std::cerr << "Cannot close socket:" << std::endl;
			std::cerr << strerror(errno) << std::endl;
		}

		if (unlink(mLocal->sun_path) != 0) {
			std::cerr << "Cannot unlink:" << std::endl;
			std::cerr << strerror(errno) << std::endl;
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
		return -1;
	}

	char const *commandAscii = command.toStdString().c_str();
	if (send(mSocket, commandAscii, strlen(commandAscii) + 1, 0) < 0) {
		std::cerr << "Cannot send a message to the daemon:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return -1;
	}

	forever {
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(mSocket, &rfds);
		struct timeval tv;
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		select(mSocket + 1, &rfds, nullptr, nullptr, &tv);
		if (FD_ISSET(mSocket, &rfds)) {
			const int bufferSize = 2048;
			char buffer[bufferSize];
			const int replyLen = recv(mSocket, buffer, bufferSize, 0);
			if (replyLen < 0) {
				std::cerr << "Cannot receive a reply from the daemon:" << std::endl;
				std::cerr << strerror(errno) << std::endl;
				return -1;
			} else if (buffer[0] == '<') { //unsolicited message
				continue;
			} else {
				buffer[replyLen] = '\0';
				reply = buffer;
				return 0;
			}
		} else {
			std::cerr << "No reply from the daemon" << std::endl;
			return -1;
		}
	}
}

bool WpaSupplicantCommunicator::isPending()
{
	struct timeval tv;
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
		std::cerr << "Cannot receive a message from the daemon:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return -1;
	}

	buffer[messageLen] = '\0';
	message = buffer;

	qDebug() << "Message: " << message;

	return 0;
}
