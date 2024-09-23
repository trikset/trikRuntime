/* Copyright 2024 CyberTech Labs Ltd.
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

#include "trikNetworkTests.h"

#include <trikControl/brickFactory.h>
#include <testUtils/wait.h>
#include <QThread>

using namespace tests;
using namespace tests::utils;

static const int port = 8889;

void TrikNetworkTests::SetUp()
{
	mBrick.reset(trikControl::BrickFactory::create("./test-system-config.xml"
			, "./test-model-config.xml", "./"));
	mMailboxInterface.reset(trikNetwork::MailboxFactory::create(port));
}

void TrikNetworkTests::TearDown()
{
}

trikNetwork::MailboxInterface *TrikNetworkTests::mailboxInterface()
{
	return mMailboxInterface.data();
}

TEST_F(TrikNetworkTests, basePropertyTests)
{
	EXPECT_EQ(false, mailboxInterface()->isConnected());
	EXPECT_EQ(false, mailboxInterface()->hasMessages());
	EXPECT_EQ("127.0.0.1", mailboxInterface()->myIp());
	EXPECT_EQ(true, mailboxInterface()->isEnabled());
}

TEST_F(TrikNetworkTests, baseHullNumberTest)
{
	mailboxInterface()->setHullNumber(2);
	EXPECT_EQ(2, mailboxInterface()->myHullNumber());
	mailboxInterface()->joinNetwork("", -1, 3);
	EXPECT_EQ(3, mailboxInterface()->myHullNumber());
}

static trikNetwork::MailboxInterface *prepareHost(int port, int hullNumber, int portToConnect)
{
	auto* host = trikNetwork::MailboxFactory::create(port);
	host->joinNetwork("127.0.0.1", portToConnect, hullNumber);
	QThread* thread = new QThread();
	QObject::connect(thread, &QThread::finished, host, &trikNetwork::MailboxInterface::deleteLater);
	QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
	host->moveToThread(thread);
	thread->start();
	return host;
}

static void receive(trikNetwork::MailboxInterface *host, QString &answer)
{
	QMetaObject::invokeMethod(host, [host, &answer](){
		answer = host->receive(true);
	});
}

static void cleanUp(QThread *thread)
{
	thread->quit();
	thread->wait();
}

static void send(trikNetwork::MailboxInterface *host, int hullNumber, const QString &message)
{
	Wait::wait(500);
	if (hullNumber == -1) {
		host->send(message);
	} else {
		host->send(hullNumber, message);
	}
	Wait::wait(500);
}

/////
///// 2 connect to 1; 2 send message to 1; 1 send message to 2
/////
TEST_F(TrikNetworkTests, baseTwoHostCommunicationTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* host = prepareHost(8890, 1, 8889);
	EXPECT_EQ(false, host->hasMessages());
	EXPECT_EQ("127.0.0.1", host->myIp());
	// It may not be able to connect by that time, and tests isEnabled() and isConnected() won't make sense.
	Wait::wait(100);
	EXPECT_EQ(true, host->isEnabled());
	EXPECT_EQ(true, host->isConnected());
	EXPECT_EQ(1, host->myHullNumber());
	EXPECT_EQ("127.0.0.1", host->serverIp());

	QString answer;
	receive(host, answer);
	send(mailboxInterface(), 1, "message to host 2");
	EXPECT_EQ("message to host 2", answer);

	mailboxInterface()->moveToThread(host->thread());
	receive(mailboxInterface(), answer);
	send(host, 3, "message to host 1");

	EXPECT_EQ("message to host 1", answer);

	auto currentThread = QThread::currentThread();
	QMetaObject::invokeMethod(mailboxInterface(), [this, currentThread](){
		mailboxInterface()->moveToThread(currentThread);
	}, Qt::BlockingQueuedConnection);

	cleanUp(host->thread());
}

/////
///// 2, 3 connect to 1; 1 send message to 2, 3;
/////
TEST_F(TrikNetworkTests, sequentialSendingMessagesTest)
{
	auto* sndHost = prepareHost(8890, 4, 8889);
	EXPECT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	EXPECT_EQ(5, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);

	send(mailboxInterface(), 4, "message to host 2");
	send(mailboxInterface(), 5, "message to host 3");

	EXPECT_EQ("message to host 2", sndAnswer);
	EXPECT_EQ("message to host 3", thdAnswer);

	cleanUp(sndHost->thread());
	cleanUp(thdHost->thread());
}

///
/// 2, 3 connect to 1 with same hull number; 1 send message to 2, 3;
///
TEST_F(TrikNetworkTests, baseMulticastTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, 4, 8889);
	EXPECT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 4, 8889);
	EXPECT_EQ(4, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);

	send(mailboxInterface(), 4, "message to all hosts");

	EXPECT_EQ("message to all hosts", sndAnswer);
	EXPECT_EQ("message to all hosts", thdAnswer);

	cleanUp(sndHost->thread());
	cleanUp(thdHost->thread());
}

///
/// Multiple iterations of sending messages between two hosts
///
TEST_F(TrikNetworkTests, twoHostCycleTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, 5, 8889);
	EXPECT_EQ(5, sndHost->myHullNumber());

	QThread* fstThread = new QThread();
	QObject::connect(fstThread, &QThread::finished, fstThread, &QThread::deleteLater);
	mailboxInterface()->moveToThread(fstThread);
	fstThread->start();

	QString fstAnswerFstHost;
	QString sndAnswerFstHost;
	QString fstAnswerSndHost;
	QString sndAnswerSndHost;

	receive(sndHost, fstAnswerSndHost);
	send(mailboxInterface(), 5, "1 message to 2 host");
	receive(mailboxInterface(), fstAnswerFstHost);
	send(sndHost, 3, "1 message to 1 host");
	receive(sndHost, sndAnswerSndHost);
	send(mailboxInterface(), 5, "2 message to 2 host");
	receive(mailboxInterface(), sndAnswerFstHost);
	send(sndHost, 3, "2 message to 1 host");

	EXPECT_EQ("1 message to 1 host", fstAnswerFstHost);
	EXPECT_EQ("1 message to 2 host", fstAnswerSndHost);
	EXPECT_EQ("2 message to 1 host", sndAnswerFstHost);
	EXPECT_EQ("2 message to 2 host", sndAnswerSndHost);

	auto threadForCleanUp = mailboxInterface()->thread();

	auto currentThread = QThread::currentThread();
	QMetaObject::invokeMethod(mailboxInterface(), [this, currentThread](){
		mailboxInterface()->moveToThread(currentThread);
	}, Qt::BlockingQueuedConnection);

	cleanUp(sndHost->thread());
	cleanUp(threadForCleanUp);

}

///
/// The host sends a message with the same hull number as its own
///
TEST_F(TrikNetworkTests, sendToIdenticalHullNumberTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, mailboxInterface()->myHullNumber(), 8889);
	EXPECT_EQ(mailboxInterface()->myHullNumber(), sndHost->myHullNumber());

	QThread* fstThread = new QThread();
	QObject::connect(fstThread, &QThread::finished, fstThread, &QThread::deleteLater);
	mailboxInterface()->moveToThread(fstThread);
	fstThread->start();

	QString fstAnswer;
	QString sndAnswer;

	receive(sndHost, sndAnswer);
	send(mailboxInterface(), mailboxInterface()->myHullNumber(), "message to 2 host");
	receive(mailboxInterface(), fstAnswer);
	send(sndHost, sndHost->myHullNumber(), "message to 1 host");

	EXPECT_EQ("message to 1 host", fstAnswer);
	EXPECT_EQ("message to 2 host", sndAnswer);

	auto threadForCleanUp = mailboxInterface()->thread();

	auto currentThread = QThread::currentThread();
	QMetaObject::invokeMethod(mailboxInterface(), [this, currentThread](){
		mailboxInterface()->moveToThread(currentThread);
	}, Qt::BlockingQueuedConnection);

	cleanUp(sndHost->thread());
	cleanUp(threadForCleanUp);

}

TEST_F(TrikNetworkTests, realMulticastWithIdenticalHullNumber)
{
	mailboxInterface()->setHullNumber(3);

	auto* sndHost = prepareHost(8890, mailboxInterface()->myHullNumber(), 8889);
	EXPECT_EQ(mailboxInterface()->myHullNumber(), sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, mailboxInterface()->myHullNumber(), 8889);
	EXPECT_EQ(mailboxInterface()->myHullNumber(), thdHost->myHullNumber());

	auto* fourthHost = prepareHost(8892, mailboxInterface()->myHullNumber(), 8889);
	EXPECT_EQ(mailboxInterface()->myHullNumber(), fourthHost->myHullNumber());

	auto* fifthHost = prepareHost(8893, mailboxInterface()->myHullNumber(), 8889);
	EXPECT_EQ(mailboxInterface()->myHullNumber(), fifthHost->myHullNumber());

	auto firstIterationMessage = "firstIteration";

	QString sndAnswer;
	receive(sndHost, sndAnswer);

	QString thdAnswer;
	receive(thdHost, thdAnswer);

	QString fourthAnswer;
	receive(fourthHost, fourthAnswer);

	QString fifthAnswer;
	receive(fifthHost, fifthAnswer);

	send(mailboxInterface(), mailboxInterface()->myHullNumber(), firstIterationMessage);

	EXPECT_EQ(firstIterationMessage, sndAnswer);
	EXPECT_EQ(firstIterationMessage, thdAnswer);
	EXPECT_EQ(firstIterationMessage, fourthAnswer);
	EXPECT_EQ(firstIterationMessage, fifthAnswer);

	receive(thdHost, thdAnswer);
	receive(fourthHost, fourthAnswer);
	receive(fifthHost, fifthAnswer);

	auto secondIterationMessage = "secondIteration";
	send(sndHost, sndHost->myHullNumber(), secondIterationMessage);

	EXPECT_EQ(secondIterationMessage, thdAnswer);
	EXPECT_EQ(secondIterationMessage, fourthAnswer);
	EXPECT_EQ(secondIterationMessage, fifthAnswer);

	receive(sndHost, sndAnswer);
	receive(fourthHost, fourthAnswer);
	receive(fifthHost, fifthAnswer);

	auto thirdIterationMessage = "thirdIteration";
	send(thdHost, thdHost->myHullNumber(), thirdIterationMessage);

	EXPECT_EQ(thirdIterationMessage, sndAnswer);
	EXPECT_EQ(thirdIterationMessage, fourthAnswer);
	EXPECT_EQ(thirdIterationMessage, fifthAnswer);

	cleanUp(sndHost->thread());
	cleanUp(thdHost->thread());
	cleanUp(fourthHost->thread());
	cleanUp(fifthHost->thread());

}

TEST_F(TrikNetworkTests, threeHostCycleTest)
{
	mailboxInterface()->setHullNumber(3);

	auto* sndHost = prepareHost(8890, 4, 8889);
	EXPECT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	EXPECT_EQ(5, thdHost->myHullNumber());

	QThread* fstThread = new QThread();
	QObject::connect(fstThread, &QThread::finished, fstThread, &QThread::deleteLater);
	mailboxInterface()->moveToThread(fstThread);
	fstThread->start();

	QString fstAnswer;
	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);
	send(mailboxInterface(), 4, "message to 2 host");
	receive(mailboxInterface(), fstAnswer);
	send(sndHost, 5, "message to 3 host");
	send(thdHost, 3, "message to 1 host");

	EXPECT_EQ("message to 1 host", fstAnswer);
	EXPECT_EQ("message to 2 host", sndAnswer);
	EXPECT_EQ("message to 3 host", thdAnswer);

	cleanUp(sndHost->thread());
	cleanUp(thdHost->thread());
	cleanUp(fstThread);
}

TEST_F(TrikNetworkTests, sendWithoutParamTest)
{
	mailboxInterface()->setHullNumber(3);

	auto* sndHost = prepareHost(8890, 4, 8889);
	EXPECT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	EXPECT_EQ(5, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);
	send(mailboxInterface(), -1, "message to all host");

	EXPECT_EQ("message to all host", sndAnswer);
	EXPECT_EQ("message to all host", thdAnswer);

	cleanUp(sndHost->thread());
	cleanUp(thdHost->thread());
}

TEST_F(TrikNetworkTests, multicastDontSendExtraPacketsTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, 4, 8889);
	EXPECT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	EXPECT_EQ(5, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);

	send(mailboxInterface(), 4, "message to all hosts");

	EXPECT_EQ("message to all hosts", sndAnswer);
	EXPECT_EQ("", thdAnswer);

	send(mailboxInterface(), 5, "message to 3 host");
	EXPECT_EQ("message to 3 host", thdAnswer);

	cleanUp(sndHost->thread());
	cleanUp(thdHost->thread());
}
