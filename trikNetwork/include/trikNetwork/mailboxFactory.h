/* Copyright 2015 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtXml/QDomElement>

#include "mailboxInterface.h"

#include "declSpec.h"

namespace trikNetwork {

/// Factory that creates real mailbox object.
class TRIKNETWORK_EXPORT MailboxFactory
{
public:
	/// Method that creates mailbox object.
	/// Transfers ownership over MailboxInterface object to caller.
	/// @param port - TCP port of mailbox server.
	static MailboxInterface *create(int port);

	/// Method that creates mailbox object.
	/// Transfers ownership over MailboxInterface object to caller.
	/// @param config - root of XML document with configuration. If configuration is incorrect, uninitialized mailbox
	///        object will be returned.
	static MailboxInterface *create(QDomElement const &config);

	/// Creates uninitialized mailbox object as a placeholder for configurations where mailbox is turned off.
	/// Transfers ownership over MailboxInterface object to caller.
	static MailboxInterface *create();
};

}
