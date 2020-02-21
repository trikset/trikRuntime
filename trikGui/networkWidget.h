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

#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>

namespace trikGui {

/// Widget, that shows current state of the network -- hostname and IP-address
class NetworkWidget : public QWidget
{
	Q_OBJECT
public:

	/// Constructor
	explicit NetworkWidget(QWidget *parent = 0);

private slots:
	void updateIP();
	void updateHostname();
	void updateHullNumber();

private:
	QVBoxLayout mNetworkLayout;

	QLabel mHostnameLabel;
	QLabel mIPLabel;
	QLabel mHullNumberLabel;

	QTimer mUpdateTimer;
};

}
