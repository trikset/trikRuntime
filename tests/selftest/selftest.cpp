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

#include <QtCore/QScopedPointer>

#include <trikControl/brickInterface.h>
#include <trikControl/brickFactory.h>
#include <trikKernel/configurer.h>
#include <trikNetwork/gamepadFactory.h>
#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikNetwork/mailboxInterface.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;
using namespace trikControl;
using namespace trikNetwork;

TEST(selftest, brickCheck)
{
	QScopedPointer<BrickInterface> brick(BrickFactory::create(*QThread::currentThread(), "./", "./"));

	ASSERT_TRUE(brick->battery() != nullptr);
	ASSERT_EQ(DeviceInterface::Status::ready, brick->battery()->status());
	ASSERT_THAT(0.0, Not(DoubleEq(brick->battery()->readVoltage())));
}

TEST(selftest, mailboxCheck)
{
	trikKernel::Configurer configurer(".");
	QScopedPointer<MailboxInterface> mailbox(MailboxFactory::create(configurer));
	ASSERT_FALSE(mailbox.isNull());
}

TEST(selftest, gamepadCheck)
{
	trikKernel::Configurer configurer(".");
	QScopedPointer<GamepadInterface> gamepad(GamepadFactory::create(configurer));
	ASSERT_FALSE(gamepad.isNull());
}
