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

#include <QObject>
#include <QTimer>

namespace trikGui {

/// Widget, that shows current state of the network -- hostname and IP-address
class Network : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString hostName READ hostName NOTIFY hostNameChanged)
	Q_PROPERTY(QString ip READ ip NOTIFY ipChanged)
	Q_PROPERTY(QString hullNumber READ hullNumber NOTIFY hullNumberChanged)

public:

	/// Constructor
	explicit Network(QObject *parent = nullptr);

private Q_SLOTS:
	void updateIP();
	void updateHostname();
	void updateHullNumber();

private:
	QString hostName();
	QString ip();
	QString hullNumber();

	QString mHostName{""};
	QString mIp{""};
	QString mHullNumber{""};

	QTimer mUpdateTimer;
Q_SIGNALS:
	/// Emitted when host name changed
	void hostNameChanged();
	/// Emitted when ip changed
	void ipChanged();
	/// Emitted when hull number changed
	void hullNumberChanged();
};

}
