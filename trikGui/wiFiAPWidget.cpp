/* Copyright 2014 CyberTech Labs Ltd.
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

#include "wiFiAPWidget.h"

#include <QtCore/QDebug>

#include "rcReader.h"

using namespace trikGui;

WiFiAPWidget::WiFiAPWidget(QWidget *parent)
	: QWidget(parent)
{
	setWindowState(Qt::WindowFullScreen);

	mTitle.setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	mParametersLayout.setAlignment(Qt::AlignCenter);
	mNetworkLabel.setAlignment(Qt::AlignCenter);
	mKeyLabel.setAlignment(Qt::AlignCenter);
	mIpLabel.setAlignment(Qt::AlignCenter);

	mTitle.setText(tr("Network parameters"));
	mNetworkLabel.setText(tr("Name not found"));
	mKeyLabel.setText(tr("Password not found"));
	mIpLabel.setText(tr("IP address: ") + QString("192.168.1.1"));

	getParameters();

	mMainLayout.addWidget(&mTitle);
	mMainLayout.addStretch(2);
	mMainLayout.addLayout(&mParametersLayout);
	mMainLayout.addStretch(1);

	mParametersLayout.addWidget(&mNetworkLabel);
	mParametersLayout.addWidget(&mKeyLabel);
	mParametersLayout.addWidget(&mIpLabel);

	setLayout(&mMainLayout);
}

void WiFiAPWidget::exec()
{
	show();
	mEventLoop.exec();
}

void WiFiAPWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Right:
		case Qt::Key_Return:
		case Qt::Key_Meta:
		case Qt::Key_Left:
		case Qt::Key_PowerDown: {
			hide();
			mEventLoop.quit();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}

void WiFiAPWidget::getParameters()
{
	RcReader rcReader("/etc/trik/trikrc");

	QString const ssid = rcReader.value("trik_wifi_ap_ssid");
	if (!ssid.isEmpty()) {
		mNetworkLabel.setText(tr("Name: ") + ssid);
	}

	QString const passphrase = rcReader.value("trik_wifi_ap_passphrase");
	if (!passphrase.isEmpty()) {
		mKeyLabel.setText(tr("Password: ") + passphrase);
	}
}
