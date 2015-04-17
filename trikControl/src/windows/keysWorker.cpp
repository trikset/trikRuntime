/* Copyright 2014 CyberTech Labs Ltd.
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

#include "src/keysWorker.h"

#include <QtCore/QDebug>

using namespace trikControl;

KeysWorker::KeysWorker(QString const &keysPath, DeviceState &state)
	: mState(state)
{
	Q_UNUSED(keysPath)
}

void KeysWorker::reset()
{
}

bool KeysWorker::wasPressed(int code)
{
	Q_UNUSED(code)

	return false;
}

void KeysWorker::readKeysEvent()
{
}
