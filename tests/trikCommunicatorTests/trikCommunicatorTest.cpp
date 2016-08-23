/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include "trikCommunicatorTest.h"

#include <trikControl/brickFactory.h>
#include <testUtils/tcpClientSimulator.h>
#include <testUtils/wait.h>

using namespace tests;
using namespace tests::utils;

static const int port = 8888;

void TrikCommunicatorTest::SetUp()
{
	mBrick.reset(trikControl::BrickFactory::create("./test-system-config.xml"
			, "./test-model-config.xml", "./"));

	mCommunicator.reset(new trikCommunicator::TrikCommunicator(*mBrick, nullptr));
	mCommunicator->startServer(port);
}

void TrikCommunicatorTest::TearDown()
{
}

TEST_F(TrikCommunicatorTest, configVersionTest)
{
	TcpClientSimulator client("127.0.0.1", port);
	client.send("configVersion");
	Wait::wait(100);
	ASSERT_EQ("configVersion: model-test", client.latestResponse());
}
