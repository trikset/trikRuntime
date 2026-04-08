/* Copyright 2015 CyberTech Labs Ltd.
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

#include <controller.h>
#include <wiFiMode.h>

namespace trikGui {

/// A label that shows wifi connection status.
class WiFiIndicator : public QObject
{
	Q_OBJECT
	Q_PROPERTY(trikGui::WiFiMode::Mode mode READ mode NOTIFY modeChanged)

public:
	/// @param controller is used to get the current network info
	explicit WiFiIndicator(Controller &controller, QObject *parent = nullptr);
	Q_INVOKABLE bool isConnected();

public Q_SLOTS:

	/// Requests connection info from the controller and updates the status.
	void updateStatus();

private:
	QTimer mUpdateTimer;
	Controller &mController;
	WiFiMode::Mode mMode{trikGui::WiFiMode::Mode::Unknown};
	WiFiMode::Mode mode();

Q_SIGNALS:
	/// Emitted when mode changed
	void modeChanged();
};

}
