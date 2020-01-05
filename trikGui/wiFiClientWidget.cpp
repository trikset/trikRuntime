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
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QMessageBox>

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include <trikWiFi/trikWiFi.h>

#include <QsLog.h>

using namespace trikGui;
using namespace trikWiFi;

static const int connectionTimeout = 5000;

WiFiClientWidget::WiFiClientWidget(TrikWiFi &trikWiFi, QWidget *parent)
	: TrikGuiDialog(parent)
	, mWiFi(trikWiFi)
	, mConnectionState(ConnectionState::notConnected)
{
	connect(&mWiFi, &TrikWiFi::scanFinished, this, &WiFiClientWidget::onNetworksInfoUpdated);
	connect(&mWiFi, &TrikWiFi::connected, this, &WiFiClientWidget::onConnected);
	connect(&mWiFi, &TrikWiFi::disconnected, this, &WiFiClientWidget::onDisconnected);
	connect(&mWiFi, &TrikWiFi::statusReady, this, &WiFiClientWidget::onStatusUpdated);
	connect(&mWiFi, &TrikWiFi::error, this, &WiFiClientWidget::onError);

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

	showScanning();

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

	mConnectionTimeoutTimer.setInterval(connectionTimeout);
	mConnectionTimeoutTimer.setSingleShot(true);

	connect(&mConnectionTimeoutTimer, &QTimer::timeout, this, &WiFiClientWidget::onConnectionTimeout);

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

void WiFiClientWidget::onNetworksInfoUpdated()
{
	mAvailableNetworksModel.clear();
	mNetworks.clear();

	for (const ScanResult &result : mWiFi.scanResult()) {
		NetworkInfo network{result.ssid, result.known, result.security};
		/// If two networks have the same ssid, only last one will be shown. Some routers boadcast ssids on different
		/// channels and they will be shown as different networks if we do not filter them here. It is perceived as a
		/// bug by users, so we sacrifice correctness in sake of simplicity.
		mNetworks.insert(network.ssid, network);
	}

	for (auto &&network : mNetworks) {
		mAvailableNetworksModel.appendRow(new QStandardItem(network.ssid));
	}

	updateConnectionStatusesInNetworkList();
}

void WiFiClientWidget::onConnected()
{
	mConnectionTimeoutTimer.stop();
	const trikWiFi::Status connectionStatus = mWiFi.statusResult();
	setConnectionStatus(ConnectionState::connected, connectionStatus.ipAddress, connectionStatus.ssid);
}

void WiFiClientWidget::onDisconnected(DisconnectReason reason)
{
	if (reason == DisconnectReason::unplanned) {
		// Maybe disconnect was caused by network going out of range, so we need to rescan networks to determine
		// what networks are left.
		mWiFi.scanRequest();
		showScanning();
	}

	// Anyway, we are disconnected, so if we are already connecting, we will probably be connected sometime, in other
	// cases we shall report disconnect.
	setConnectionStatus(
			mConnectionState == ConnectionState::connecting
					? mConnectionState
					: ConnectionState::notConnected
			, ""
			, "");
}

void WiFiClientWidget::onStatusUpdated()
{
	const trikWiFi::Status connectionStatus = mWiFi.statusResult();
	setConnectionStatus(connectionStatus.connected ? ConnectionState::connected : ConnectionState::notConnected
			, connectionStatus.ipAddress, connectionStatus.ssid);
}

void WiFiClientWidget::onError(const QString &message)
{
	QLOG_ERROR() << message;
	if (message == "statusRequest") {
		setConnectionStatus(ConnectionState::errored, "", "");
	}
}

void WiFiClientWidget::onConnectionTimeout()
{
	setConnectionStatus(ConnectionState::errored, "", "");
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

void WiFiClientWidget::setConnectionStatus(ConnectionState state, const QString &ip, const QString &ssid)
{
	QPixmap pixmap;
	mConnectionState = state;
	mCurrentSsid = "";

	switch (mConnectionState) {
	case ConnectionState::connected:
		pixmap.load("://resources/connected.png");
		mIpValueLabel.setText(ip);
		mCurrentSsid = ssid;
		break;
	case ConnectionState::connecting:
		pixmap.load("://resources/unknownConnectionStatus.png");
		mIpValueLabel.setText(tr("connecting..."));
		break;
	case ConnectionState::notConnected:
		pixmap.load("://resources/notConnected.png");
		mIpValueLabel.setText(tr("no connection"));
		break;
	case ConnectionState::errored:
		pixmap.load("://resources/notConnected.png");
		mIpValueLabel.setText(tr("error"));
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

		QFont font = item->font();
		font.setBold(false);
		item->setFont(font);
		if (item->text() == mCurrentSsid) {
			item->setIcon(QIcon(QPixmap("://resources/connectedWifi.png")));
			font.setBold(true);
			item->setFont(font);
		} else if (mNetworks[item->text()].isKnown) {
			item->setIcon(QIcon(QPixmap("://resources/knownWifi.png")));
		} else if (mNetworks[item->text()].security == Security::none) {
			item->setIcon(QIcon(QPixmap("://resources/openWifi.png")));
		} else {
			item->setIcon(QIcon(QPixmap("://resources/passwordedWifi.png")));
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
		// Trying to connect to "Scanning..." label, for example.
		return;
	}

	const auto doConnect = [&]() {
		setConnectionStatus(ConnectionState::connecting, "", "");
		mWiFi.connect(ssid);
		mConnectionTimeoutTimer.start();
	};

	if (mNetworks[ssid].isKnown) {
		doConnect();
	} else if (mNetworks[ssid].security == Security::none) {
		QMessageBox confirmMessageBox(QMessageBox::Warning, tr("Confirm connection")
				, tr("Are you sure you want to connect to open WiFi network?"), QMessageBox::Yes | QMessageBox::No);
		confirmMessageBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
		const int result = confirmMessageBox.exec();
		if (result == QMessageBox::Yes) {
			doConnect();
			mNetworks[ssid].isKnown = true;
		}
	}
}

void WiFiClientWidget::showScanning()
{
	const auto scanning = new QStandardItem(tr("Scanning..."));
	scanning->setIcon(QIcon(QPixmap("://resources/wait.png")));
	mAvailableNetworksModel.clear();
	mAvailableNetworksModel.appendRow(scanning);
}
