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

#include "wiFiClientWidget.h"

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

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include <trikWiFi/trikWiFi.h>

#include <QsLog.h>

using namespace trikGui;

using namespace trikWiFi;

WiFiClientWidget::WiFiClientWidget(TrikWiFi &trikWiFi, QWidget *parent)
	: TrikGuiDialog(parent)
	, mWiFi(trikWiFi)
	, mConnectionState(ConnectionState::notConnected)
{
	connect(&mWiFi, SIGNAL(scanFinished()), this, SLOT(scanForAvailableNetworksDoneSlot()));
	connect(&mWiFi, SIGNAL(connected()), this, SLOT(connectedSlot()));
	connect(&mWiFi, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));
	connect(&mWiFi, SIGNAL(statusReady()), this, SLOT(statusReadySlot()));
	connect(&mWiFi, SIGNAL(error(const QString &)), this, SLOT(errorSlot(const QString &)));

	mConnectionIconLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	mIpLabel.setText(tr("IP:"));
	mIpLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	mNameLabel.setText(tr("Name:"));
	mNameLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
	mNameValueLabel.setText(name);

	mAvailableNetworksLabel.setText(tr("Available networks:"));

	mAvailableNetworksView.setModel(&mAvailableNetworksModel);
	mAvailableNetworksView.setSelectionMode(QAbstractItemView::SingleSelection);

	const auto scanning = new QStandardItem(tr("Scanning..."));
	scanning->setIcon(QIcon("://resources/wait.png"));
	mAvailableNetworksModel.appendRow(scanning);

	mIpAddressLayout.addWidget(&mConnectionIconLabel);
	mIpAddressLayout.addWidget(&mIpLabel);
	mIpAddressLayout.addWidget(&mIpValueLabel);

	mNameLayout.addWidget(&mNameLabel);
	mNameLayout.addWidget(&mNameValueLabel);

	mMainLayout.addLayout(&mIpAddressLayout);
	mMainLayout.addLayout(&mNameLayout);
	mMainLayout.addWidget(&mAvailableNetworksLabel);
	mMainLayout.addWidget(&mAvailableNetworksView);
	setLayout(&mMainLayout);

	mWiFi.statusRequest();
	mWiFi.scanRequest();
}

WiFiClientWidget::~WiFiClientWidget()
{
}

void WiFiClientWidget::renewFocus()
{
	mAvailableNetworksView.setFocus();
}

void WiFiClientWidget::scanForAvailableNetworksDoneSlot()
{
	mAvailableNetworksModel.clear();
	mNetworks.clear();

	for (const ScanResult &result : mWiFi.scanResult()) {
		NetworkInfo network{result.ssid, result.networkId, result.security};
		/// If two networks have te same ssid, only last one will be shown. Some routers boadcast ssids on different
		/// channels and they will be shown as different networks if we do not filter them here. It is perceived as a
		/// bug by users, so we sacrifice correctness in sake of simplicity.
		mNetworks.insert(network.ssid, network);
	}

	for (const NetworkInfo &network : mNetworks.values()) {
		mAvailableNetworksModel.appendRow(new QStandardItem(network.ssid));
	}

	updateConnectionStatusesInNetworkList();
}

void WiFiClientWidget::connectedSlot()
{
	mConnectionState = ConnectionState::connected;
	setConnectionStatus(mWiFi.statusResult());
}

void WiFiClientWidget::disconnectedSlot()
{
	mConnectionState = ConnectionState::notConnected;

	mWiFi.statusRequest();

	// Now to determine reason of disconnect --- maybe the network is out of range now.
	mWiFi.scanRequest();
}

void WiFiClientWidget::statusReadySlot()
{
	const Status connectionStatus = mWiFi.statusResult();
	mConnectionState = connectionStatus.connected ? ConnectionState::connected : ConnectionState::notConnected;
	setConnectionStatus(connectionStatus);
}

void WiFiClientWidget::errorSlot(const QString &message)
{
	QLOG_ERROR() << message;
	if (message == "statusRequest") {
		mConnectionState = ConnectionState::errored;
		setConnectionStatus(trikWiFi::Status());
	}
}

void WiFiClientWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Return: {
		connectToSelectedNetwork();
		break;
	}
	default: {
		TrikGuiDialog::keyPressEvent(event);
		break;
	}
	}
}

void WiFiClientWidget::setConnectionStatus(const trikWiFi::Status &status)
{
	QPixmap pixmap;
	switch (mConnectionState) {
	case ConnectionState::connected:
		pixmap.load("://resources/connected.png");
		mIpValueLabel.setText(status.ipAddress);
		mCurrentSsid = status.ssid;
		break;
	case ConnectionState::connecting:
		pixmap.load("://resources/unknownConnectionStatus.png");
		mIpValueLabel.setText(tr("connecting..."));
		mCurrentSsid = "";
		break;
	case ConnectionState::notConnected:
		pixmap.load("://resources/notConnected.png");
		mIpValueLabel.setText(tr("no connection"));
		mCurrentSsid = "";
		break;
	case ConnectionState::errored:
		pixmap.load("://resources/notConnected.png");
		mIpValueLabel.setText(tr("error"));
		mCurrentSsid = "";
		break;
	}

	mConnectionIconLabel.setPixmap(pixmap);
	updateConnectionStatusesInNetworkList();
}

void WiFiClientWidget::updateConnectionStatusesInNetworkList()
{
	for (int i = 0; i < mAvailableNetworksModel.rowCount(); ++i) {
		QStandardItem * const item = mAvailableNetworksModel.item(i);
		if (item->text() == tr("Scanning...")) {
			return;
		}

		qDebug() << item->text() << ":" << mNetworks[item->text()].wpaSupplicantId << (int)mNetworks[item->text()].security;

		QFont font = item->font();
		font.setBold(false);
		item->setFont(font);
		if (item->text() == mCurrentSsid) {
			item->setIcon(QIcon("://resources/connectedWifi.png"));
			font.setBold(true);
			item->setFont(font);
		} else if (mNetworks[item->text()].wpaSupplicantId != -1) {
			item->setIcon(QIcon("://resources/knownWifi.png"));
		} else if (mNetworks[item->text()].security == Security::none) {
			item->setIcon(QIcon("://resources/openWifi.png"));
		} else {
			item->setIcon(QIcon("://resources/passwordedWifi.png"));
		}
	}

	mAvailableNetworksView.setFocus();
	mAvailableNetworksView.selectionModel()->select(
			mAvailableNetworksModel.index(0, 0)
			, QItemSelectionModel::ClearAndSelect
			);

	mAvailableNetworksView.setCurrentIndex(mAvailableNetworksModel.index(0, 0));
}

void WiFiClientWidget::connectToSelectedNetwork()
{
	const QModelIndexList selected = mAvailableNetworksView.selectionModel()->selectedIndexes();
	if (selected.size() != 1) {
		return;
	}

	const QString ssid = mAvailableNetworksModel.itemFromIndex(selected[0])->text();
	if (ssid == mCurrentSsid) {
		return;
	}

	if (!mNetworks.contains(ssid)) {
		return;
	}

	mConnectionState = ConnectionState::connecting;

	setConnectionStatus(Status());

	if (mNetworks[ssid].wpaSupplicantId != -1) {
		mWiFi.connect(mNetworks[ssid].wpaSupplicantId);
	} else if (mNetworks[ssid].security == Security::none) {
		mWiFi.connectToOpenNetwork(ssid);
	}
}
