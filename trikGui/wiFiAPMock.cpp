/* Copyright 2024 Daniel Chehade.
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

#include "wiFiAPMock.h"
#include "QsLog.h"
#include <QDir>
#include <QrCodeGenerator.h>
#include <trikKernel/paths.h>
using namespace trikGui;

WiFiAPMock::WiFiAPMock(QObject *parent) : QObject(parent) {

	if (!mNetworkName.isEmpty() && !mPassword.isEmpty() && !mIpValue.isEmpty()) {
		QrCodeGenerator generator;
		QString data = "WIFI:T:WPA;S:" + mNetworkName + ";P:" + mPassword + ";H:;";
		QImage qrCodeImage = generator.generateQr(data, 300);
		QDir dir(trikKernel::Paths::imagesPath());

		if (!dir.exists() && !dir.mkpath(trikKernel::Paths::imagesPath())) {
			QLOG_ERROR() << "Cannot create directory for images";
		} else {
			const QString name = trikKernel::Paths::imagesPath() + "qrCodeImg.png";
			qrCodeImage.save(name);
			mNameImage = name;
			Q_EMIT nameImageChanged();
		}
	}
}

QString WiFiAPMock::networkName() { return mNetworkName; }
QString WiFiAPMock::password() { return mPassword; }
QString WiFiAPMock::ipValue() { return mIpValue; }
void WiFiAPMock::setQmlParent(QObject *parent) { setParent(parent); }

QString WiFiAPMock::nameImage() { return mNameImage; }
