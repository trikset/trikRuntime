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
	ASSERT_EQ(false, mailboxInterface()->isConnected());
	ASSERT_EQ(false, mailboxInterface()->hasMessages());
	ASSERT_EQ("127.0.0.1", mailboxInterface()->myIp());
	ASSERT_EQ(true, mailboxInterface()->isEnabled());
}

TEST_F(TrikNetworkTests, baseHullNumberTest)
{
	mailboxInterface()->setHullNumber(2);
	ASSERT_EQ(2, mailboxInterface()->myHullNumber());
	mailboxInterface()->joinNetwork("", -1, 3);
	ASSERT_EQ(3, mailboxInterface()->myHullNumber());
}

static trikNetwork::MailboxInterface *prepareHost(int port, int hullNumber, int portToConnect)
{
	auto* host = trikNetwork::MailboxFactory::create(port);
	host->joinNetwork("127.0.0.1", portToConnect, hullNumber);
	QThread* thread = new QThread();
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

static void cleanUp(trikNetwork::MailboxInterface *host)
{
	auto* thread = host->thread();
	host->deleteLater();
	thread->quit();
	if (!thread->wait(1000)) {
		qDebug() << "Unable to stop thread" << thread;
	}
	else {
		delete thread;
	}
}

static void cleanUp(QThread *thread)
{
	thread->quit();
	if (!thread->wait(1000)) {
		qDebug() << "Unable to stop thread" << thread;
	}
	else {
		delete thread;
	}
}

static void send(trikNetwork::MailboxInterface *host, int hullNumber, const QString &message)
{
	Wait::wait(1000);
	if (hullNumber == -1) {
		host->send(message);
	} else {
		host->send(hullNumber, message);
	}
	Wait::wait(1000);
}

/////
///// 2 connect to 1; 2 send message to 1; 1 send message to 2
/////
TEST_F(TrikNetworkTests, baseTwoHostCommunicationTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* host = prepareHost(8890, 1, 8889);
	ASSERT_EQ(false, host->hasMessages());
	ASSERT_EQ("127.0.0.1", host->myIp());
	ASSERT_EQ(true, host->isEnabled());
	ASSERT_EQ(true, host->isConnected());
	ASSERT_EQ(1, host->myHullNumber());
	ASSERT_EQ("127.0.0.1", host->serverIp());

	QString answer;
	receive(host, answer);
	send(mailboxInterface(), 1, "message to host 2");
	ASSERT_EQ("message to host 2", answer);

	mailboxInterface()->moveToThread(host->thread());
	receive(mailboxInterface(), answer);
	send(host, 3, "message to host 1");

	ASSERT_EQ("message to host 1", answer);

	auto currentThread = QThread::currentThread();
	QMetaObject::invokeMethod(mailboxInterface(), [this, currentThread](){
		mailboxInterface()->moveToThread(currentThread);
	}, Qt::BlockingQueuedConnection);

	cleanUp(host);
}

/////
///// 2, 3 connect to 1; 1 send message to 2, 3;
/////
TEST_F(TrikNetworkTests, sequentialSendingMessagesTest)
{
	auto* sndHost = prepareHost(8890, 4, 8889);
	ASSERT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	ASSERT_EQ(5, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);

	send(mailboxInterface(), 4, "message to host 2");
	send(mailboxInterface(), 5, "message to host 3");

	ASSERT_EQ("message to host 2", sndAnswer);
	ASSERT_EQ("message to host 3", thdAnswer);

	cleanUp(sndHost);
	cleanUp(thdHost);
}

///
/// 2, 3 connect to 1 with same hull number; 1 send message to 2, 3;
///
TEST_F(TrikNetworkTests, baseMulticastTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, 4, 8889);
	ASSERT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 4, 8889);
	ASSERT_EQ(4, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);

	send(mailboxInterface(), 4, "message to all hosts");

	ASSERT_EQ("message to all hosts", sndAnswer);
	ASSERT_EQ("message to all hosts", thdAnswer);

	cleanUp(sndHost);
	cleanUp(thdHost);
}

///
/// Multiple iterations of sending messages between two hosts
///
TEST_F(TrikNetworkTests, twoHostCycleTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, 5, 8889);
	ASSERT_EQ(5, sndHost->myHullNumber());

	QThread* fstThread = new QThread();
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

	ASSERT_EQ("1 message to 1 host", fstAnswerFstHost);
	ASSERT_EQ("1 message to 2 host", fstAnswerSndHost);
	ASSERT_EQ("2 message to 1 host", sndAnswerFstHost);
	ASSERT_EQ("2 message to 2 host", sndAnswerSndHost);

	auto threadForCleanUp = mailboxInterface()->thread();

	auto currentThread = QThread::currentThread();
	QMetaObject::invokeMethod(mailboxInterface(), [this, currentThread](){
		mailboxInterface()->moveToThread(currentThread);
	}, Qt::BlockingQueuedConnection);

	cleanUp(sndHost);
	cleanUp(threadForCleanUp);

}

///
/// The host sends a message with the same hull number as its own
///
TEST_F(TrikNetworkTests, sendToIdenticalHullNumberTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, mailboxInterface()->myHullNumber(), 8889);
	ASSERT_EQ(mailboxInterface()->myHullNumber(), sndHost->myHullNumber());

	QThread* fstThread = new QThread();
	mailboxInterface()->moveToThread(fstThread);
	fstThread->start();

	QString fstAnswer;
	QString sndAnswer;

	receive(sndHost, sndAnswer);
	send(mailboxInterface(), mailboxInterface()->myHullNumber(), "message to 2 host");
	receive(mailboxInterface(), fstAnswer);
	send(sndHost, sndHost->myHullNumber(), "message to 1 host");

	ASSERT_EQ("message to 1 host", fstAnswer);
	ASSERT_EQ("message to 2 host", sndAnswer);

	auto threadForCleanUp = mailboxInterface()->thread();

	auto currentThread = QThread::currentThread();
	QMetaObject::invokeMethod(mailboxInterface(), [this, currentThread](){
		mailboxInterface()->moveToThread(currentThread);
	}, Qt::BlockingQueuedConnection);

	cleanUp(sndHost);
	cleanUp(threadForCleanUp);

}

TEST_F(TrikNetworkTests, realMulticastWithIdenticalHullNumber)
{
	mailboxInterface()->setHullNumber(3);

	auto* sndHost = prepareHost(8890, mailboxInterface()->myHullNumber(), 8889);
	ASSERT_EQ(mailboxInterface()->myHullNumber(), sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, mailboxInterface()->myHullNumber(), 8889);
	ASSERT_EQ(mailboxInterface()->myHullNumber(), thdHost->myHullNumber());

	auto* fourthHost = prepareHost(8892, mailboxInterface()->myHullNumber(), 8889);
	ASSERT_EQ(mailboxInterface()->myHullNumber(), fourthHost->myHullNumber());

	auto* fifthHost = prepareHost(8893, mailboxInterface()->myHullNumber(), 8889);
	ASSERT_EQ(mailboxInterface()->myHullNumber(), fifthHost->myHullNumber());

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

	ASSERT_EQ(firstIterationMessage, sndAnswer);
	ASSERT_EQ(firstIterationMessage, thdAnswer);
	ASSERT_EQ(firstIterationMessage, fourthAnswer);
	ASSERT_EQ(firstIterationMessage, fifthAnswer);

	receive(thdHost, thdAnswer);
	receive(fourthHost, fourthAnswer);
	receive(fifthHost, fifthAnswer);

	auto secondIterationMessage = "secondIteration";
	send(sndHost, sndHost->myHullNumber(), secondIterationMessage);

	ASSERT_EQ(secondIterationMessage, thdAnswer);
	ASSERT_EQ(secondIterationMessage, fourthAnswer);
	ASSERT_EQ(secondIterationMessage, fifthAnswer);

	receive(sndHost, sndAnswer);
	receive(fourthHost, fourthAnswer);
	receive(fifthHost, fifthAnswer);

	auto thirdIterationMessage = "thirdIteration";
	send(thdHost, thdHost->myHullNumber(), thirdIterationMessage);

	ASSERT_EQ(thirdIterationMessage, sndAnswer);
	ASSERT_EQ(thirdIterationMessage, fourthAnswer);
	ASSERT_EQ(thirdIterationMessage, fifthAnswer);

	cleanUp(sndHost);
	cleanUp(thdHost);
	cleanUp(fourthHost);
	cleanUp(fifthHost);

}

TEST_F(TrikNetworkTests, threeHostCycleTest)
{
	mailboxInterface()->setHullNumber(3);

	auto* sndHost = prepareHost(8890, 4, 8889);
	ASSERT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	ASSERT_EQ(5, thdHost->myHullNumber());

	QThread* fstThread = new QThread();
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

	ASSERT_EQ("message to 1 host", fstAnswer);
	ASSERT_EQ("message to 2 host", sndAnswer);
	ASSERT_EQ("message to 3 host", thdAnswer);

	cleanUp(sndHost);
	cleanUp(thdHost);
	cleanUp(fstThread);
}

TEST_F(TrikNetworkTests, sendWithoutParamTest)
{
	mailboxInterface()->setHullNumber(3);

	auto* sndHost = prepareHost(8890, 4, 8889);
	ASSERT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	ASSERT_EQ(5, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);
	send(mailboxInterface(), -1, "message to all host");

	ASSERT_EQ("message to all host", sndAnswer);
	ASSERT_EQ("message to all host", thdAnswer);

	cleanUp(sndHost);
	cleanUp(thdHost);
}

TEST_F(TrikNetworkTests, multicastDontSendExtraPacketsTest)
{
	mailboxInterface()->setHullNumber(3);
	auto* sndHost = prepareHost(8890, 4, 8889);
	ASSERT_EQ(4, sndHost->myHullNumber());

	auto* thdHost = prepareHost(8891, 5, 8889);
	ASSERT_EQ(5, thdHost->myHullNumber());

	QString sndAnswer;
	QString thdAnswer;

	receive(sndHost, sndAnswer);
	receive(thdHost, thdAnswer);

	send(mailboxInterface(), 4, "message to all hosts");

	ASSERT_EQ("message to all hosts", sndAnswer);
	ASSERT_EQ("", thdAnswer);

	send(mailboxInterface(), 5, "message to 3 host");
	ASSERT_EQ("message to 3 host", thdAnswer);

	cleanUp(sndHost);
	cleanUp(thdHost);
}
