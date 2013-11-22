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
#include <trikWiFi/wpaConfigurer.h>

using namespace trikGui;

using namespace trikWiFi;

NetConfigWidget::NetConfigWidget(QWidget *parent)
	: QWidget(parent)
	, mWiFi(new TrikWiFi("/tmp/trikwifi", "/run/wpa_supplicant/wlan0", this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowState(Qt::WindowFullScreen);

	WpaConfigurer::configureWpaSupplicant("wpa-config.xml", *mWiFi);

	QList<NetworkConfiguration> const networksFromWpaSupplicant = mWiFi->listNetworks();
	foreach (NetworkConfiguration const &networkConfiguration, networksFromWpaSupplicant) {
		mNetworksAvailableForConnection.insert(networkConfiguration.ssid, networkConfiguration.id);
	}

	connect(mWiFi.data(), SIGNAL(scanFinished()), this, SLOT(scanForAvailableNetworksDone()));
	mWiFi->scan();

	mConnectionIconLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	mIpLabel.setText(tr("IP:"));
	mIpLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	mAvailableNetworksLabel.setText(tr("Available networks:"));

	mAvailableNetworksView.setModel(&mAvailableNetworksModel);
	mAvailableNetworksView.setSelectionMode(QAbstractItemView::SingleSelection);

	mIpAddressLayout.addWidget(&mConnectionIconLabel);
	mIpAddressLayout.addWidget(&mIpLabel);
	mIpAddressLayout.addWidget(&mIpValueLabel);

	mMainLayout.addLayout(&mIpAddressLayout);
	mMainLayout.addWidget(&mAvailableNetworksLabel);
	mMainLayout.addWidget(&mAvailableNetworksView);
	setLayout(&mMainLayout);

	Status const connectionStatus = mWiFi->status();
	setConnectionStatus(connectionStatus);
}

NetConfigWidget::~NetConfigWidget()
{
}

QString NetConfigWidget::menuEntry()
{
	return tr("Network config");
}

void NetConfigWidget::scanForAvailableNetworksDone()
{
	mAvailableNetworksItems.clear();
	foreach (ScanResult const &result, mWiFi->scanResults()) {
		mAvailableNetworksItems.append(new QStandardItem(result.ssid));
	}

	updateConnectionStatusesInNetworkList();
}

void NetConfigWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Meta:
	case Qt::Key_Left: {
		close();
		break;
	}
	case Qt::Key_Enter: {
		connectToSelectedNetwork();
	}
	default: {
		QWidget::keyPressEvent(event);
		break;
	}
	}
}

void NetConfigWidget::setConnectionStatus(trikWiFi::Status const &status)
{
	if (status.connected) {
		QPixmap pixmap("://resources/connected.png");
		mConnectionIconLabel.setPixmap(pixmap);
		mIpValueLabel.setText(status.ipAddress);
		mCurrentSsid = status.ssid;
	} else {
		QPixmap pixmap("://resources/notConnected.png");
		mConnectionIconLabel.setPixmap(pixmap);
		mIpValueLabel.setText(tr("no connection"));
		mCurrentSsid = "";
	}

	updateConnectionStatusesInNetworkList();
}

void NetConfigWidget::updateConnectionStatusesInNetworkList()
{
	foreach (QStandardItem * const item, mAvailableNetworksItems) {
		if (item->text() == mCurrentSsid) {
			item->setIcon(QIcon("://resources/connectedToNetwork.png"));
		} else if (mNetworksAvailableForConnection.contains(item->text())) {
			item->setIcon(QIcon("://resources/notConnectedToNetwork.png"));
		} else {
			item->setIcon(QIcon("://resources/connectionToNetworkImpossible.png"));
		}
	}

	mAvailableNetworksModel.clear();
	mAvailableNetworksModel.appendColumn(mAvailableNetworksItems);
}

void NetConfigWidget::connectToSelectedNetwork()
{
	QModelIndexList const selected = mAvailableNetworksView.selectionModel()->selectedIndexes();
	if (selected.size() != 1) {
		return;
	}

	QString const ssid = mAvailableNetworksModel.itemFromIndex(selected[0])->text();
	if (ssid == mCurrentSsid) {
		return;
	}

	if (!mNetworksAvailableForConnection.contains(ssid)) {
		return;
	}

	mWiFi->connect(mNetworksAvailableForConnection[ssid]);

	updateConnectionStatusesInNetworkList();
}
