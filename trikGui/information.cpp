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

#include "information.h"
#include <trikKernel/version.h>

#include <QFile>
#include <QtNetwork/QNetworkInterface>

using namespace trikGui;

Information::Information(QObject *parent) : QObject(parent) {}

QString Information::macAddress() const {
	QString mac = "";

	for (auto &&interface : QNetworkInterface::allInterfaces()) {
		if (interface.name() == "wlan0") {
			mac = interface.hardwareAddress();
			break;
		}
	}
	return mac;
}

QString Information::osVersion() const {
	QFile version("/etc/version");
	if (!version.exists()) {
		return "";
	}

	version.open(QIODevice::ReadOnly);

	return QString::fromUtf8(version.readAll()).trimmed();
}

void Information::setQmlParent(QObject *parent)
{
	setParent(parent);
}
