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
#ifdef Q_OS_LINUX
#include <sys/socket.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "trikWiFiWorker.h"

using namespace trikWiFi;

TrikWiFi::TrikWiFi(const QString &interfaceFilePrefix
		, const QString &daemonFile
		, QObject *parent)
	: QObject(parent)
	, mWorker(new TrikWiFiWorker(interfaceFilePrefix, daemonFile))
{
	qRegisterMetaType<trikWiFi::DisconnectReason>("trikWiFi::DisconnectReason");
	mWorker->moveToThread(&mWorkerThread);

	QObject::connect(mWorker.data(), &TrikWiFiWorker::scanFinished
					 , this, &TrikWiFi::scanFinished);
	QObject::connect(mWorker.data(), &TrikWiFiWorker::connected
					 , this, &TrikWiFi::connected);
	QObject::connect(mWorker.data(), &TrikWiFiWorker::disconnected
			, this, &TrikWiFi::disconnected);
	QObject::connect(mWorker.data(), &TrikWiFiWorker::statusReady
					 , this, &TrikWiFi::statusReady);
	QObject::connect(mWorker.data(), &TrikWiFiWorker::error
					 , this, &TrikWiFi::error);

	QLOG_INFO() << "Starting TrikWiFi worker thread" << &mWorkerThread;

	mWorkerThread.setObjectName(mWorker->metaObject()->className());
	mWorkerThread.start();

	reinit();
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
	QMetaObject::invokeMethod(mWorker.data(), &TrikWiFiWorker::reinit);
}

void TrikWiFi::dispose()
{
	QMetaObject::invokeMethod(mWorker.data(), &TrikWiFiWorker::dispose);
}

SignalStrength TrikWiFi::signalStrength()
{
#if 0 // defined(Q_OS_LINUX)
	iwreq req;
	auto iwname = "wlan0";
	strcpy(req.ifr_name, iwname);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	req.u.data.pointer = (iw_statistics *)malloc(sizeof(iw_statistics));
	req.u.data.length = sizeof(iw_statistics);

	if (ioctl(sockfd, SIOCGIWSTATS, &req) == -1) {
		QLOG_ERROR() << "Access to invalid interface about signal strength";
		close(sockfd);
		return SignalStrength::undefined;
	} else if (((iw_statistics *)req.u.data.pointer)->qual.updated & IW_QUAL_DBM) {
		auto level = ((iw_statistics *)req.u.data.pointer)->qual.level - 256;
		close(sockfd);
		if (level < -70) {
			return SignalStrength::low;
		} else if (level > -50) {
			return SignalStrength::high;
		} else {
			return SignalStrength::medium;
		}
	}
#else
	return SignalStrength::undefined;
#endif
}

void TrikWiFi::connect(const QString &ssid)
{
	QMetaObject::invokeMethod(mWorker.data(), [this, &ssid](){mWorker->connect(ssid);});
}

void TrikWiFi::disconnect()
{
	QMetaObject::invokeMethod(mWorker.data(), &TrikWiFiWorker::disconnect);
}

void TrikWiFi::statusRequest()
{
	QMetaObject::invokeMethod(mWorker.data(), &TrikWiFiWorker::statusRequest);
}

Status TrikWiFi::statusResult() const
{
	return mWorker->statusResult();
}

void TrikWiFi::scanRequest()
{
	QMetaObject::invokeMethod(mWorker.data(), &TrikWiFiWorker::scanRequest);
}

QList<ScanResult> TrikWiFi::scanResult() const
{
	return mWorker->scanResult();
}
