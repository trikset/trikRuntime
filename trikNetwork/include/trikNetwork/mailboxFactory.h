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

#include "trikNetworkDeclSpec.h"

namespace trikKernel {
class Configurer;
}

namespace trikNetwork {

class MailboxInterface;

/*
 * Each mailboxServer contains a server that listens for connections on an open port.
 * This mailboxServer is desired to be used both in the visual language of TRIKStudio and in the textual interpretation.
 * However, for the expected connection, we need to reuse the same open port, existing connections, etc.
 * For this purpose, a weak_ptr to the already existing mailbox is stored, and in the case of invalidity, it is recreated.
*/

/// Factory that creates real mailbox object.
class TRIKNETWORK_EXPORT MailboxFactory
{
public:
	/// Method that creates mailbox object.
	/// Transfers ownership over MailboxInterface object to caller.
	/// @param port - TCP port of mailbox server.
	static QSharedPointer<MailboxInterface> create(int port);

	/// Method that creates mailbox object.
	/// Transfers ownership over MailboxInterface object to caller.
	/// @param configurer - configurer object that contains preparsed XML config. If configuration is incorrect,
	///        uninitialized mailbox object will be returned.
	static QSharedPointer<MailboxInterface> create(const trikKernel::Configurer &configurer);
private:

	static QSharedPointer<MailboxInterface> prepare(int port);

	static QHash<int, QWeakPointer<MailboxInterface>> mailbox;
};

}
