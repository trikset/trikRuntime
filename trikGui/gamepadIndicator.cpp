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

#include "trikKernel/paths.h"

using namespace trikGui;

GamepadIndicator::GamepadIndicator(Controller &controller, QWidget *parent)
	: QLabel(parent)
	, mController(controller)
{
	connect(&mController, SIGNAL(gamepadDisconnected()), this, SLOT(setOff()));
	connect(&mController, SIGNAL(gamepadConnected()), this, SLOT(setOn()));

	updateStatus();
	connect(&mUpdateTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	mUpdateTimer.start(7000);
}

void GamepadIndicator::setOn()
{
	QPixmap icon("://resources/gamepad_on.png");
	setPixmap(icon);
	show();
}

void GamepadIndicator::setOff()
{
	hide();
}

void GamepadIndicator::connected(bool connected)
{
	connected ? setOn() : setOff();
}

void GamepadIndicator::updateStatus()
{
	connected(mController.gamepadConnectionStatus());
}

