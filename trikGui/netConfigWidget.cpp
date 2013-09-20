/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "netConfigWidget.h"

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QScrollBar>

using namespace trikGui;

NetConfigWidget::NetConfigWidget(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowState(Qt::WindowFullScreen);

	mTitleLabel.setText("Network Configuration");

	generateNetConfigList();
	mConfigModel.appendColumn(mConfigItems);

	mConfigView.setModel(&mConfigModel);

	mLayout.addWidget(&mTitleLabel);
	mLayout.addWidget(&mConfigView);
	setLayout(&mLayout);
}

NetConfigWidget::~NetConfigWidget()
{
}

QString NetConfigWidget::menuEntry()
{
	return "Show network configuration";
}

void NetConfigWidget::generateNetConfigList()
{
	foreach (QNetworkInterface const &interface, QNetworkInterface::allInterfaces()) {
		mConfigItems.append(new QStandardItem(interface.name()));
		foreach (const QNetworkAddressEntry &entry, interface.addressEntries()) {
			if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
				mConfigItems.append(new QStandardItem(QString("IP address: ")));
				mConfigItems.append(new QStandardItem(entry.ip().toString()));
//				mConfigItems.append(new QStandardItem(QString("Netmask: ")));
//				mConfigItems.append(new QStandardItem(entry.netmask().toString()));
			}
		}

//		mConfigItems.append(new QStandardItem(QString("Hardware address: ")));
//		mConfigItems.append(new QStandardItem(interface.hardwareAddress()));
		mConfigItems.append(new QStandardItem(QString()));
	}
}

void NetConfigWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Meta: case Qt::Key_Enter: {
			close();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
