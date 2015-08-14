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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
#else
	#include <QtWidgets/QApplication>
#endif

#include <unistd.h>

#include <trikKernel/configurer.h>

#include "keysWorker.h"

using namespace trikControl;

Keys::Keys(const trikKernel::Configurer &configurer)
{
	mKeysWorker.reset(new KeysWorker(configurer.attributeByDevice("keys", "deviceFile"), mState));
	if (!mState.isFailed()) {
		connect(mKeysWorker.data(), SIGNAL(buttonPressed(int, int)), this, SIGNAL(buttonPressed(int, int)));
		connect(mKeysWorker.data(), SIGNAL(buttonPressed(int, int)), this, SLOT(changeButtonState(int, int)));
		mKeysWorker->moveToThread(&mWorkerThread);
		mWorkerThread.start();
		mState.ready();
	}
}

Keys::~Keys()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

Keys::Status Keys::status() const
{
	return mState.status();
}

void Keys::reset()
{
	mKeysWorker->reset();
}

bool Keys::wasPressed(int code)
{
	return mKeysWorker->wasPressed(code);
}

bool Keys::isPressed(int code)
{
	return mKeysPressed.value(code, false);
}

void Keys::changeButtonState(int code, int value)
{
	mKeysPressed[code] = value;
}

int Keys::buttonCode(bool wait)
{
	if (!wait) {
		return pressedButton();
	}

	while (true) {
		int code = pressedButton();
		if (code == -1) {
			usleep(20);
			QApplication::processEvents();
		} else {
			return code;
		}
	}
}

int Keys::pressedButton()
{
	for (int button : mKeysPressed.keys()) {
		if (mKeysPressed[button]) {
			return button;
		}
	}

	return -1;
}
