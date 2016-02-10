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

#include "trikWiFi.h"

#include <QsLog.h>

#include "trikWiFiWorker.h"

using namespace trikWiFi;

TrikWiFi::TrikWiFi(const QString &interfaceFilePrefix
		, const QString &daemonFile
		, QObject *parent)
	: QObject(parent)
	, mWorker(new TrikWiFiWorker(interfaceFilePrefix, daemonFile))
{
	mWorker->moveToThread(&mWorkerThread);

	QObject::connect(mWorker.data(), SIGNAL(scanFinished()), this, SIGNAL(scanFinished()));
	QObject::connect(mWorker.data(), SIGNAL(connected()), this, SIGNAL(connected()));
	QObject::connect(mWorker.data(), SIGNAL(disconnected()), this, SIGNAL(disconnected()));

	QObject::connect(mWorker.data(), SIGNAL(statusReady()), this, SIGNAL(statusReady()));
	QObject::connect(mWorker.data(), SIGNAL(listNetworksReady()), this, SIGNAL(listNetworksReady()));

	QObject::connect(mWorker.data(), SIGNAL(error(QString)), this, SIGNAL(error(QString)));

	QLOG_INFO() << "Starting TrikWiFi worker thread" << &mWorkerThread;

	mWorkerThread.start();

	QMetaObject::invokeMethod(mWorker.data(), "reinit");
}

TrikWiFi::~TrikWiFi()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

void TrikWiFi::reinit()
{
	QMetaObject::invokeMethod(mWorker.data(), "reinit");
}

void TrikWiFi::dispose()
{
	QMetaObject::invokeMethod(mWorker.data(), "dispose");
}

void TrikWiFi::connect(int id)
{
	QMetaObject::invokeMethod(mWorker.data(), "connect", Q_ARG(int, id));
}

void TrikWiFi::disconnect()
{
	QMetaObject::invokeMethod(mWorker.data(), "disconnect");
}

void TrikWiFi::statusRequest()
{
	QMetaObject::invokeMethod(mWorker.data(), "statusRequest");
}

Status TrikWiFi::statusResult() const
{
	return mWorker->statusResult();
}

void TrikWiFi::scanRequest()
{
	QMetaObject::invokeMethod(mWorker.data(), "scanRequest");
}

QList<ScanResult> TrikWiFi::scanResult() const
{
	return mWorker->scanResult();
}

void TrikWiFi::listNetworksRequest()
{
	QMetaObject::invokeMethod(mWorker.data(), "listNetworksRequest");
}

QList<NetworkConfiguration> TrikWiFi::listNetworksResult() const
{
	return mWorker->listNetworksResult();
}
