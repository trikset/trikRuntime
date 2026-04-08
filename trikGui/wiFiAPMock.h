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

#pragma once

#include <QObject>

namespace trikGui {

/// Widget showing mock parameters of wireless access point on the controller.
class WiFiAPMock : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString networkName READ networkName NOTIFY networkNameChanged)
	Q_PROPERTY(QString password READ password NOTIFY passwordChanged)
	Q_PROPERTY(QString ipValue READ ipValue NOTIFY ipValueChanged)
	Q_PROPERTY(QString nameImage READ nameImage NOTIFY nameImageChanged)

public:
	/// Constructor
	explicit WiFiAPMock(QObject *parent = nullptr);
	Q_INVOKABLE void setQmlParent(QObject *parent);

private:
	QString mNetworkName{"trik-f6fec0"};
	QString mPassword{"54124221"};
	QString mIpValue{"192.168.77.1"};

	QString networkName();
	QString password();
	QString ipValue();
	QString mNameImage;
	QString nameImage();
Q_SIGNALS:
	/// Emitted when network name changed
	void networkNameChanged();
	/// Emitted when password changed
	void passwordChanged();
	/// Emitted when ip value changed
	void ipValueChanged();
	/// Emitted when name image changed
	void nameImageChanged();
};

} // namespace trikGui
