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

using namespace trikWiFi;

struct sockaddr_un
{
	int x;
};

WpaSupplicantCommunicator::WpaSupplicantCommunicator(
		const QString &interfaceFile
		, const QString &daemonFile
		, QObject *parent
		)
	: QObject(parent)
	, mSocket(-1)
	, mLocal(new sockaddr_un())
	, mDest(new sockaddr_un())
{
	Q_UNUSED(interfaceFile);
	Q_UNUSED(daemonFile);
}

WpaSupplicantCommunicator::~WpaSupplicantCommunicator() = default;

int WpaSupplicantCommunicator::fileDescriptor() const
{
	return -1;
}

int WpaSupplicantCommunicator::attach() const
{
	return -1;
}

int WpaSupplicantCommunicator::detach() const
{
	return -1;
}

int WpaSupplicantCommunicator::request(const QString &command, QString &reply) const
{
	Q_UNUSED(command);
	Q_UNUSED(reply);

	return -1;
}

bool WpaSupplicantCommunicator::isPending() const
{
	return false;
}

int WpaSupplicantCommunicator::receive(QString &message) const
{
	Q_UNUSED(message);

	return 0;
}
