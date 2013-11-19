/* Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#include "netConfigWidget.h"

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>
#include <QtGui/QKeyEvent>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QScrollBar>
#else
	#include <QtWidgets/QScrollBar>
#endif

#include <QtCore/QDebug>

#include <trikWiFi/trikWiFi.h>

using namespace trikGui;

using namespace trikWiFi;

NetConfigWidget::NetConfigWidget(QWidget *parent)
	: QWidget(parent)
	, mWiFi(new TrikWiFi("/tmp/trikwifi", "/run/wpa_supplicant/wlan0", this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowState(Qt::WindowFullScreen);

	connect(mWiFi.data(), SIGNAL(scanFinished()), this, SLOT(scanForAvailableNetworksDone()));
	int const result = mWiFi->scan();

	qDebug() << result;

	mTitleLabel.setText(tr("Network Config"));

	mConnectionIconLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QPixmap pixmap("://resources/connected.png");
	mConnectionIconLabel.setPixmap(pixmap);

	mIpLabel.setText(tr("IP:"));
	mIpLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	mIpValueLabel.setText("127.0.0.1");

	mAvailableNetworksModel.appendColumn(mAvailableNetworksItems);
	mAvailableNetworksView.setModel(&mAvailableNetworksModel);

//	mConfigView.setModel(&mConfigModel);

	mIpAddressLayout.addWidget(&mConnectionIconLabel);
	mIpAddressLayout.addWidget(&mIpLabel);
	mIpAddressLayout.addWidget(&mIpValueLabel);

	mMainLayout.addWidget(&mTitleLabel);
	mMainLayout.addLayout(&mIpAddressLayout);
	mMainLayout.addWidget(&mAvailableNetworksLabel);
	mMainLayout.addWidget(&mAvailableNetworksView);
	setLayout(&mMainLayout);
}

NetConfigWidget::~NetConfigWidget()
{
}

QString NetConfigWidget::menuEntry()
{
	return tr("Network config");
}

//void NetConfigWidget::generateNetConfigList()
//{
//	foreach (QNetworkInterface const &interface, QNetworkInterface::allInterfaces()) {
//		if (interface.flags() & QNetworkInterface::IsLoopBack) {
//			continue;
//		}

//		mConfigItems.append(new QStandardItem(interface.name()));
//		foreach (const QNetworkAddressEntry &entry, interface.addressEntries()) {
//			if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
//			{
//				mConfigItems.append(new QStandardItem(QString(tr("IP address: "))));
//				mConfigItems.append(new QStandardItem(entry.ip().toString()));
//			}
//		}

//		mConfigItems.append(new QStandardItem(QString()));
//	}
//}

void NetConfigWidget::scanForAvailableNetworksDone()
{
	mAvailableNetworksItems.append(new QStandardItem("Ololo, scan done"));
	foreach (TrikWiFi::ScanResult const &result, mWiFi->scanResults()) {
		mAvailableNetworksItems.append(new QStandardItem(result.ssid));
	}
}

void NetConfigWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Meta:
	case Qt::Key_Enter: {
		close();
		break;
	}
	default: {
		QWidget::keyPressEvent(event);
		break;
	}
	}
}
