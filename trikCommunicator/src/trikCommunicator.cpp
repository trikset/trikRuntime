/* Copyright 2013 Yurii Litvinov
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

#include "trikCommunicator.h"

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <trikScriptRunner/trikScriptRunner.h>

#include "src/connection.h"

using namespace trikCommunicator;

TrikCommunicator::TrikCommunicator(trikControl::Brick &brick, QString const &startDirPath)
	: mTrikScriptRunner(new trikScriptRunner::TrikScriptRunner(brick, startDirPath))
	, mHasScriptRunnerOwnership(true)
{
	init();
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner)
	: mTrikScriptRunner(&runner)
	, mHasScriptRunnerOwnership(false)
{
	init();
}

TrikCommunicator::~TrikCommunicator()
{
	foreach (QThread *thread, mConnections.keys()) {
		thread->quit();
		if (!thread->wait(1000)) {
			qDebug() << "Unable to stop thread" << thread;
		}
	}

	qDeleteAll(mConnections);
	qDeleteAll(mConnections.keys());

	if (mHasScriptRunnerOwnership) {
		delete mTrikScriptRunner;
	}
}

void TrikCommunicator::startServer(int const &port)
{
	if (!listen(QHostAddress::Any, port)) {
		qDebug() << "Can not start TrikCommunicator server";
	} else {
		qDebug() << "TrikCommunicator started";
	}
}

void TrikCommunicator::incomingConnection(int socketDescriptor)
{
	qDebug() << "New connection, socket descriptor: " << socketDescriptor;

	QThread * const connectionThread = new QThread();

	connect(connectionThread, SIGNAL(finished()), connectionThread, SLOT(deleteLater()));
	connect(connectionThread, SIGNAL(finished()), this, SLOT(onConnectionClosed()));

	Connection * const connectionWorker = new Connection();

	connect(connectionWorker, SIGNAL(startedDirectScript()), this, SIGNAL(startedDirectScript()));
	connect(connectionWorker, SIGNAL(startedScript(QString)), this, SIGNAL(startedScript(QString)));

	connectionWorker->moveToThread(connectionThread);

	mConnections.insert(connectionThread, connectionWorker);

	connectionThread->start();

	QMetaObject::invokeMethod(connectionWorker, "init", Q_ARG(int, socketDescriptor)
			, Q_ARG(trikScriptRunner::TrikScriptRunner *, mTrikScriptRunner));
}

void TrikCommunicator::onConnectionClosed()
{
	QThread * const thread = static_cast<QThread *>(sender());

	// Thread shall already be finished here.
	delete mConnections.value(thread);

	mConnections.remove(thread);
}

void TrikCommunicator::init()
{
	qRegisterMetaType<trikScriptRunner::TrikScriptRunner *>("trikScriptRunner::TrikScriptRunner *");

	connect(mTrikScriptRunner, SIGNAL(completed()), this, SIGNAL(finishedScript()));
}
