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

#include "src/scriptRunnerWrapper.h"
#include "src/connection.h"

using namespace trikCommunicator;

TrikCommunicator::TrikCommunicator(QString const &configFilePath)
	: mScriptRunnerWrapper(new ScriptRunnerWrapper(configFilePath))
{
	qRegisterMetaType<ScriptRunnerWrapper *>("ScriptRunnerWrapper *");
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner)
	: mScriptRunnerWrapper(new ScriptRunnerWrapper(runner))
{
	qRegisterMetaType<ScriptRunnerWrapper *>("ScriptRunnerWrapper *");
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
	connectionWorker->moveToThread(connectionThread);

	mConnections.insert(connectionThread, connectionWorker);

	connectionThread->start();

	QMetaObject::invokeMethod(connectionWorker, "init", Q_ARG(int, socketDescriptor)
			, Q_ARG(ScriptRunnerWrapper *, mScriptRunnerWrapper.data()));
}

void TrikCommunicator::onConnectionClosed()
{
	QThread * const thread = static_cast<QThread *>(sender());

	// Thread shall already be finished here.
	delete mConnections.value(thread);

	mConnections.remove(thread);
}
