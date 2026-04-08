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

#include <QEventLoop>
#include <QQmlApplicationEngine>
#include <trikKernel/rcReader.h>
#include <trikWiFi/trikWiFi.h>

namespace trikGui {

/// Widget which allows to set mock wi-fi mode (client or access point) and then
/// opens corresponding configuration widget.
class WiFiModeMock : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString initStatus READ initStatus NOTIFY initStatusChanged)

public:
	enum class Mode { AccessPoint, Client, Unknown };
	Q_ENUM(Mode)

	/// Constructor
	explicit WiFiModeMock(QQmlApplicationEngine *engine, QObject *parent = nullptr);

	Q_INVOKABLE void setQmlParent(QObject *parent);
	Q_INVOKABLE Mode currentMode();
	Q_INVOKABLE void createWiFiClient();
	Q_INVOKABLE void createWiFiAP();
	Q_INVOKABLE void setMode(Mode mode);

private:
	QQmlApplicationEngine *mEngine;
	QString mInitStatus;
	Mode mCurrentMode;
	QString initStatus();

Q_SIGNALS:
	/// Emitted when init status changed
	void initStatusChanged();
};

} // namespace trikGui
