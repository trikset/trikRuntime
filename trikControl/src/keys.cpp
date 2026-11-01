/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include "keys.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

using namespace trikControl;

Keys::Keys(const trikKernel::Configurer &configurer, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Keys")
{
	Q_UNUSED(configurer)
	Q_UNUSED(hardwareAbstraction)
	mState.ready();
	QLOG_INFO() << "Keys initialized (virtual, no physical buttons)";
}

Keys::~Keys() = default;

Keys::Status Keys::status() const
{
	return mState.status();
}

void Keys::reset()
{
	mMutex.lock();
	mKeysPressed.clear();
	mWasPressed.clear();
	mMutex.unlock();
}

bool Keys::wasPressed(int code)
{
	QMutexLocker lock(&mMutex);
	if (mWasPressed.contains(code)) {
		mWasPressed.remove(code);
		return true;
	}
	return false;
}

bool Keys::isPressed(int code)
{
	QMutexLocker lock(&mMutex);
	return mKeysPressed.value(code, false);
}

void Keys::emulateKeyPress(int code)
{
	QMutexLocker lock(&mMutex);
	mKeysPressed[code] = 1;
	mWasPressed.insert(code);
	mWaitCondition.wakeOne();
	Q_EMIT buttonPressed(code, 1);
	Q_EMIT buttonStateChanged();
}

int Keys::buttonCode(bool wait)
{
	if (wait) {
		QMutexLocker lock(&mMutex);
		while (mWasPressed.isEmpty()) {
			mWaitCondition.wait(&mMutex);
		}
		int code = *mWasPressed.begin();
		mWasPressed.remove(code);
		return code;
	}

	return pressedButton();
}

int Keys::pressedButton()
{
	QMutexLocker lock(&mMutex);
	for (int button : mKeysPressed.keys()) {
		if (mKeysPressed[button]) {
			return button;
		}
	}

	return -1;
}
