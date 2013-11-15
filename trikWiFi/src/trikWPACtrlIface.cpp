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

#include "trikWPACtrlIface.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <QtCore/QString>
#include <QtCore/QDebug>

TrikWPACtrlIface::TrikWPACtrlIface(const QString &interfaceFile, const QString &daemonFile, QObject *parent) :
	QObject(parent)
{
	mSocket = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (mSocket < 0)
	{
		std::cerr << "Cannot create a socket:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return;
	}

	mLocal.sun_family = AF_UNIX;
	snprintf(mLocal.sun_path, sizeof(mLocal.sun_path), "%s", interfaceFile.toAscii().constData());
	if (bind(mSocket, reinterpret_cast<sockaddr *>(&mLocal), sizeof(mLocal)) != 0)
	{
		std::cerr << "Cannot bind a name to a socket:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		close(mSocket);
		mSocket = -1;
		return;
	}

	mDest.sun_family = AF_UNIX;
	snprintf(mDest.sun_path, sizeof(mDest.sun_path), "%s", daemonFile.toAscii().constData());
	if (::connect(mSocket, reinterpret_cast<sockaddr *>(&mDest), sizeof(mDest)) != 0)
	{
		std::cerr << "Cannot connect a socket:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		unlink(mLocal.sun_path);
		close(mSocket);
		mSocket = -1;
		return;
	}

	int flags = fcntl(mSocket, F_GETFL);
	if (flags >= 0)
	{
		flags |= O_NONBLOCK;
		fcntl(mSocket, F_SETFL, flags);
	}
}

TrikWPACtrlIface::~TrikWPACtrlIface()
{
	if (mSocket >= 0)
	{
		unlink(mLocal.sun_path);
		close(mSocket);
	}
}

int TrikWPACtrlIface::fileDescriptor()
{
	return mSocket;
}

int TrikWPACtrlIface::attach()
{
	if (mSocket < 0)
		return -1;

	QString command = "ATTACH";
	QString reply;
	int res = request(command, reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

int TrikWPACtrlIface::detach()
{
	if (mSocket < 0)
		return -1;

	QString command = "DETACH";
	QString reply;
	int res = request(command, reply);
	if (res == 0 && reply == "OK\n")
		return 0;
	else
		return -1;
}

int TrikWPACtrlIface::request(const QString &command, QString &reply)
{
	if (mSocket < 0)
		return -1;

	const char *commandAscii = command.toAscii().constData();
	if (send(mSocket, commandAscii, strlen(commandAscii) + 1, 0) < 0)
	{
		std::cerr << "Cannot send a message to the daemon:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return -1;
	}
	qDebug() << "Command: " << command;

	forever
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(mSocket, &rfds);
		struct timeval tv;
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		select(mSocket + 1, &rfds, NULL, NULL, &tv);
		if (FD_ISSET(mSocket, &rfds))
		{
			const int bufferSize = 2048;
			char buffer[bufferSize];
			int replyLen = recv(mSocket, buffer, bufferSize, 0);
			if (replyLen < 0)
			{
				std::cerr << "Cannot receive a reply from the daemon:" << std::endl;
				std::cerr << strerror(errno) << std::endl;
				return -1;
			}
			else if (buffer[0] == '<') //unsolicited message
				continue;
			else
			{
				buffer[replyLen] = '\0';
				reply = buffer;
				qDebug() << "Reply: " << reply;
				return 0;
			}
		}
		else
		{
			std::cerr << "No reply from the daemon" << std::endl;
			return -1;
		}
	}
}

bool TrikWPACtrlIface::isPending()
{
	struct timeval tv;
	fd_set rfds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(mSocket, &rfds);
	select(mSocket + 1, &rfds, NULL, NULL, &tv);
	return FD_ISSET(mSocket, &rfds);
}

int TrikWPACtrlIface::receive(QString &message)
{
	int bufferSize = 256;
	char buffer[bufferSize];
	int messageLen = recv(mSocket, buffer, bufferSize, 0);
	if (messageLen < 0)
	{
		std::cerr << "Cannot receive a message from the daemon:" << std::endl;
		std::cerr << strerror(errno) << std::endl;
		return -1;
	}
	buffer[messageLen] = '\0';
	message = buffer;
	qDebug() << "Message: " << message;
	return 0;
}
