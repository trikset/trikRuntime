/* Copyright 2014 Roman Kurbatov
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
 */

#include "wiFiAPWidget.h"

#include <QtCore/QDebug>

using namespace trikGui;

WiFiAPWidget::WiFiAPWidget(QWidget *parent)
	: QWidget(parent)
	, mConfigurationFile("/etc/hostapd.conf")
{
	setWindowState(Qt::WindowFullScreen);

	mTitle.setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	mParametersLayout.setAlignment(Qt::AlignCenter);
	mNetworkLabel.setAlignment(Qt::AlignCenter);
	mKeyLabel.setAlignment(Qt::AlignCenter);
	mIpLabel.setAlignment(Qt::AlignCenter);

	mTitle.setText(tr("Network parameters"));
	mNetworkLabel.setText(tr("SSID not found"));
	mKeyLabel.setText(tr("Key not found"));
	mIpLabel.setText(tr("IP address: ") + QString("192.168.1.1"));

	if (!mConfigurationFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Can't open hostapd configuration file";
	} else {
		getParameters();
	}

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
	forever {
		QString line = mConfigurationFile.readLine();
		if (line.isEmpty()) {
			break;
		}

		int const commentStart = line.indexOf('#');
		if (commentStart >= 0) {
			line.truncate(commentStart);
		}

		int const equalsSign = line.indexOf('=');
		if (equalsSign < 0) {
			continue;
		}

		QString const option = line.left(equalsSign);
		QString const value = line.mid(equalsSign + 1);

		if (option == "ssid") {
			mNetworkLabel.setText(tr("SSID: ") + value);
		} else if (option == "wpa_passphrase") {
			mKeyLabel.setText(tr("Key: ") + value);
		}
	}
}
