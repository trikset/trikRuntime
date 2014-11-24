/* Copyright 2013 - 2014 Matvey Bryksin, Yurii Litvinov, CyberTech Labs Ltd.
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

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QHash>
#include <QtCore/QSet>

#include "declSpec.h"

namespace trikControl {

class TcpConnector;

/// Class to support remote control of a robot using TCP client.
class TRIKCONTROL_EXPORT Gamepad : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - TCP port of a gamepad server.
	Gamepad(int port);

	/// Destructor declared here for QScopedPointer to be able to clean up forward-declared TcpConnector.
	virtual ~Gamepad();

public slots:
	/// Clear data about previous pad events.
	void reset();

	/// Returns true, if given pad button, and clears "pressed" state for that button.
	bool buttonWasPressed(int buttonNumber);

	/// Returns current state of the pad, true if pressed.
	bool isPadPressed(int pad);

	/// Returns current X coordinate of given pad or -1 if this pad is not pressed.
	int padX(int pad);

	/// Returns current Y coordinate of given pad or -1 if this pad is not pressed.
	int padY(int pad);

signals:
	/// @todo ??!
	void padUp(int pad);
	void wheel(int percent);
	void pad(int pad, int x, int y);
	void button(int button, int pressed);
	void disconnect();

private slots:
	void parse(QString const &message);

private:
	Q_DISABLE_COPY(Gamepad)

	struct PadStatus {
		int x;
		int y;
		bool isPressed;
	};

	QScopedPointer<TcpConnector> mListener;
	QThread mNetworkThread;

	QSet<int> mButtonWasPressed;
	QHash<int, PadStatus> mPads;
};

}
