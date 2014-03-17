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

#include "netInitWidget.h"

#include <cstdlib>

using namespace trikGui;

NetInitWidget::NetInitWidget(QWidget *parent)
	: QWidget(parent)
	, mInitMessage(tr("Network initialization in process"))
	, mWaitMessage(tr("Please wait"))
{
	setWindowState(Qt::WindowFullScreen);

	mInitMessage.setAlignment(Qt::AlignCenter);
	mWaitMessage.setAlignment(Qt::AlignCenter);

	mLayout.addWidget(&mInitMessage);
	mLayout.addWidget(&mWaitMessage);
	setLayout(&mLayout);
}

void NetInitWidget::init(NetConfigWidget::NetworkMode mode)
{
	switch (mode) {
		case NetConfigWidget::accessPoint: {
			system("killall udhcpc");
			system("killall wpa_supplicant");
			system("ifconfig wlan0 192.168.1.1 netmask 255.255.255.0");
			system("hostapd -B /etc/hostapd.conf");
			system("udhcpd");
		}
		case NetConfigWidget::client: {
			system("killall udhcpd");
			system("killall hostapd");
			system("wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf");
			system("udhcpc -i wlan0");
		}
	}
}
