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

#include "netConfigWidget.h"

#include <QtGui/QKeyEvent>
#include <cstdlib>

#include "wiFiClientWidget.h"

using namespace trikGui;

NetConfigWidget::NetConfigWidget(QString const &configPath, NetConfigWidget::NetworkMode &currentMode, QWidget *parent)
	: QWidget(parent)
	, mConfigPath(configPath)
	, mMode(currentMode)
	, mTitle(tr("Choose mode:"))
{
	setWindowState(Qt::WindowFullScreen);

	mModes.addItem(tr("Wi-Fi client"));
	mModes.addItem(tr("Wi-Fi access point"));

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mModes);
	setLayout(&mLayout);

	mModes.selectionModel()->select(
			mMenuModel.index(0, 0)
			, QItemSelectionModel::ClearAndSelect
			);
}

void NetConfigWidget::exec()
{
	show();
	mEventLoop.exec();
}

QString NetConfigWidget::menuEntry()
{
	return tr("Network config");
}

void NetConfigWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return:
		case Qt::Key_Right: {
			if (mModes.currentItem()->text() == tr("Wi-Fi client")) {
				setClient();
			} else {
				setAccessPoint();
			}
			break;
		}
		case Qt::Key_Left:
		case Qt::Key_PowerDown:
		case Qt::Key_Meta: {
			mEventLoop.quit();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}

void NetConfigWidget::setClient()
{
	if (mMode != client) {
		mMode = client;
		system("killall udhcpd");
		system("killall hostapd");
		system("wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf");
		system("udhcpc -i wlan0");
	}

	WiFiClientWidget wiFiClientWidget(mConfigPath);

	wiFiClientWidget.exec();

	mEventLoop.quit();
}

void NetConfigWidget::setAccessPoint()
{
	if (mMode != accessPoint) {
		mMode = accessPoint;
		system("killall udhcpc");
		system("killall wpa_supplicant");
		system("ifconfig wlan0 192.168.1.1 netmask 255.255.255.0");
		system("ifconfig wlan0 up");
		system("hostapd -B /etc/hostapd.conf");
		system("udhcpd");
	}

	mEventLoop.quit();
}
