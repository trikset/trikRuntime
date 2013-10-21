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

using namespace trikGui;

NetConfigWidget::NetConfigWidget(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowState(Qt::WindowFullScreen);

	mTitleLabel.setText(tr("Network Config"));

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
	return tr("Network config");
}

void NetConfigWidget::generateNetConfigList()
{
	foreach (QNetworkInterface const &interface, QNetworkInterface::allInterfaces()) {
		if (interface.flags() & QNetworkInterface::IsLoopBack) {
			continue;
		}

		mConfigItems.append(new QStandardItem(interface.name()));
		foreach (const QNetworkAddressEntry &entry, interface.addressEntries()) {
			if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
			{
				mConfigItems.append(new QStandardItem(QString(tr("IP address: "))));
				mConfigItems.append(new QStandardItem(entry.ip().toString()));
			}
		}

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
