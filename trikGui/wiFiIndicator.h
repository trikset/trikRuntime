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

#include <QLabel>

#include <controller.h>
#include <wiFiModeWidget.h>

namespace trikGui {

/// A label that shows wifi connection status.
class WiFiIndicator : public QLabel
{
	Q_OBJECT
public:
	/// @param controller is used to get the current network info
	explicit WiFiIndicator(Controller &controller, QWidget *parent = 0);

private slots:
	/// Updates the status to 'connected'.
	void setOn();

	/// Updates the status to 'disconnected'.
	void setOff();

	/// Updates the status to 'access point mode'.
	void setAPOn();

	void setLowStrength();

	void setMediumStrength();

	void setHighStrength();

	/// Updates the status according to mode and connected parameters.
	void changeMode(WiFiModeWidget::Mode mode);

	/// Requests connection info from the controller and updates the status.
	void updateStatus();

private:
	QTimer mUpdateTimer;
	Controller &mController;
	WiFiModeWidget::Mode mMode { WiFiModeWidget::Mode::unknown };
};

}
