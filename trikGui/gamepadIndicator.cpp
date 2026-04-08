/* Copyright 2016 Anna Kudryashova
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

#include "gamepadIndicator.h"

using namespace trikGui;

GamepadIndicator::GamepadIndicator(Controller &controller, QObject *parent)
	: QObject(parent)
	, mController(controller)
{
	connect(&mController, &Controller::gamepadDisconnected, this, &GamepadIndicator::setOff);
	connect(&mController, &Controller::gamepadConnected, this, &GamepadIndicator::setOn);
}

void GamepadIndicator::setOn()
{
	mIsConnected = true;
	Q_EMIT isConnectedChanged();
}

void GamepadIndicator::setOff()
{
	mIsConnected = false;
	Q_EMIT isConnectedChanged();
}

void GamepadIndicator::connected(bool connected)
{
	connected ? setOn() : setOff();
}

bool GamepadIndicator::isConnected()
{
	return mIsConnected;
}
