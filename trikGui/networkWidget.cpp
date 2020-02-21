/* Copyright 2019 CyberTech Labs Ltd.
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

#include "networkWidget.h"

#include <QWidget>
#include <QNetworkInterface>
#include <QtCore/QSettings>

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>

using namespace trikKernel;
using namespace trikGui;

NetworkWidget::NetworkWidget(QWidget *parent)
	: QWidget(parent)
{
	mHostnameLabel.setText("Name: ");
	mIPLabel.setText("IP: ");
	mHullNumberLabel.setText(tr("Hull number:"));
	mNetworkLayout.addWidget(&mHostnameLabel);
	mNetworkLayout.addWidget(&mIPLabel);
	mNetworkLayout.addWidget(&mHullNumberLabel);
	setLayout(&mNetworkLayout);

	mUpdateTimer.setInterval(5000);
	connect(&mUpdateTimer, &QTimer::timeout, this, &NetworkWidget::updateIP);
	connect(&mUpdateTimer, &QTimer::timeout, this, &NetworkWidget::updateHostname);
	connect(&mUpdateTimer, &QTimer::timeout, this, &NetworkWidget::updateHullNumber);
	mUpdateTimer.start();
}

void NetworkWidget::updateIP()
{
	const auto & interfaces = QNetworkInterface::allInterfaces();
	const auto & interface = std::find_if(interfaces.begin(), interfaces.end()
				, [](const QNetworkInterface & interface) {return interface.name() == "wlan0"; });

	if (interface != interfaces.end()) {
		const auto & entries = interface[0].addressEntries();
		const auto & entry = std::find_if(entries.begin(), entries.end()
			, [](const QNetworkAddressEntry & entry) { return entry.ip().protocol() == QAbstractSocket::IPv4Protocol;});
		if (entry != entries.end()) {
			mIPLabel.setText(tr("IP: ") + entry[0].ip().toString());
		}
	}
}

void NetworkWidget::updateHostname()
{
	const QString name = trikKernel::FileUtils::readFromFile(trikKernel::Paths::hostnameName()).trimmed();
	mHostnameLabel.setText(tr("Name: ") + name);
}

void NetworkWidget::updateHullNumber()
{
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	auto hullNumber = settings.value("hullNumber");
	mHullNumberLabel.setText(tr("Hull number:") + " " + QString::number(hullNumber.toInt()));
}
