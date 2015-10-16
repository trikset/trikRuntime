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

#include "src/gamepadServer.h"

#include "src/gamepadConnection.h"

using namespace trikNetwork;

GamepadServer::GamepadServer()
	: TrikServer([this] () { return connectionFactory(); })
{
	setObjectName("GamepadServer");
}

Connection *GamepadServer::connectionFactory()
{
	GamepadConnection * const connection = new GamepadConnection();

	connect(connection, SIGNAL(button(int,int)),this, SIGNAL(button(int,int)));
	connect(connection, SIGNAL(pad(int,int,int)),this, SIGNAL(pad(int,int,int)));
	connect(connection, SIGNAL(padUp(int)),this, SIGNAL(padUp(int)));
	connect(connection, SIGNAL(wheel(int)),this, SIGNAL(wheel(int)));
	connect(connection, SIGNAL(disconnected(Connection *)),this, SLOT(onGamepadConnectionClosed()));

	return connection;
}

void GamepadServer::onGamepadConnectionClosed()
{
	if (activeConnections() == 0) {
		emit disconnect();
	}
}
