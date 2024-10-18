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

#include "mailboxFactory.h"

#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>
#include "mailbox.h"

using namespace trikNetwork;

QHash<int, QWeakPointer<MailboxInterface>> MailboxFactory::mailbox;

QSharedPointer<MailboxInterface> MailboxFactory::prepare(int port)
{
	auto sharedMailbox = QSharedPointer<MailboxInterface>(new Mailbox(port));
	mailbox.insert(port, sharedMailbox.toWeakRef());
	return sharedMailbox;
}

QSharedPointer<MailboxInterface> MailboxFactory::create(int port)
{
	if (mailbox.contains(port)) {
		auto sharedMailbox = mailbox[port].toStrongRef();

		if (!sharedMailbox) {
			return prepare(port);
		}

		return sharedMailbox;
	}

	return prepare(port);

}

QSharedPointer<MailboxInterface> MailboxFactory::create(const trikKernel::Configurer &configurer)
{	
	if (configurer.isEnabled("mailbox")) {

		bool ok = false;
		const int port = configurer.attributeByDevice("mailbox", "port").toInt(&ok);
		if (!ok) {
			throw trikKernel::MalformedConfigException("Incorrect mailbox port");
		}

		return create(port);

	} else {
		return nullptr;
	}
}
