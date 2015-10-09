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
#include <trikWiFi/wpaConfigurer.h>

using namespace trikGui;

using namespace trikWiFi;

WiFiClientWidget::WiFiClientWidget(const QString &configPath, TrikWiFi &trikWiFi, QWidget *parent)
	: TrikGuiDialog(parent)
	, mWiFi(trikWiFi)
	, mConnectionState(notConnected)
{
	WpaConfigurer::configureWpaSupplicant(configPath + "wpa-config.xml", mWiFi);

	const QList<NetworkConfiguration> networksFromWpaSupplicant = mWiFi.listNetworks();
	for (const NetworkConfiguration &networkConfiguration : networksFromWpaSupplicant) {
		mNetworksAvailableForConnection.insert(networkConfiguration.ssid, networkConfiguration.id);
	}

	connect(&mWiFi, SIGNAL(scanFinished()), this, SLOT(scanForAvailableNetworksDoneSlot()));
	connect(&mWiFi, SIGNAL(connected()), this, SLOT(connectedSlot()));
	connect(&mWiFi, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));

	mWiFi.scan();

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

	Status const connectionStatus = mWiFi.status();
	mConnectionState = connectionStatus.connected ? connected : notConnected;
	setConnectionStatus(connectionStatus);
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
	for (const ScanResult &result : mWiFi.scanResults()) {
		mAvailableNetworksModel.appendRow(new QStandardItem(result.ssid));
	}

	updateConnectionStatusesInNetworkList();
}

void WiFiClientWidget::connectedSlot()
{
	mConnectionState = connected;
	setConnectionStatus(mWiFi.status());
}

void WiFiClientWidget::disconnectedSlot()
{
	if (mConnectionState != connecting) {
		mConnectionState = notConnected;
	}

	setConnectionStatus(mWiFi.status());

	// Now to determine reason of disconnect --- maybe the network is out of range now.
	mWiFi.scan();
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
	case connected:
		pixmap.load("://resources/connected.png");
		mIpValueLabel.setText(status.ipAddress);
		mCurrentSsid = status.ssid;
		break;
	case connecting:
		pixmap.load("://resources/unknownConnectionStatus.png");
		mIpValueLabel.setText(tr("connecting..."));
		mCurrentSsid = "";
		break;
	case notConnected:
		pixmap.load("://resources/notConnected.png");
		mIpValueLabel.setText(tr("no connection"));
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
		QFont font = item->font();
		font.setBold(false);
		item->setFont(font);
		if (item->text() == mCurrentSsid) {
			item->setIcon(QIcon("://resources/connectedToNetwork.png"));
			font.setBold(true);
			item->setFont(font);
		} else if (mNetworksAvailableForConnection.contains(item->text())) {
			item->setIcon(QIcon("://resources/notConnectedToNetwork.png"));
		} else {
			item->setIcon(QIcon("://resources/connectionToNetworkImpossible.png"));
		}
	}

	mAvailableNetworksView.setFocus();
	mAvailableNetworksView.selectionModel()->select(
			mAvailableNetworksModel.index(0, 0)
			, QItemSelectionModel::ClearAndSelect
			);
}

void WiFiClientWidget::connectToSelectedNetwork()
{
	QModelIndexList const selected = mAvailableNetworksView.selectionModel()->selectedIndexes();
	if (selected.size() != 1) {
		return;
	}

	const QString ssid = mAvailableNetworksModel.itemFromIndex(selected[0])->text();
	if (ssid == mCurrentSsid) {
		return;
	}

	if (!mNetworksAvailableForConnection.contains(ssid)) {
		return;
	}

	mConnectionState = connecting;

	setConnectionStatus(Status());

	mWiFi.connect(mNetworksAvailableForConnection[ssid]);
}
