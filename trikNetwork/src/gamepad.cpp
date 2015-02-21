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

#include "gamepad.h"

#include <QtCore/QStringList>

#include <trikKernel/exceptions/malformedConfigException.h>
#include <QsLog.h>

#include "src/gamepadServer.h"

using namespace trikNetwork;

Gamepad::Gamepad(int port)
{
	init(port);
}

Gamepad::Gamepad(trikKernel::Configurer const &configurer)
{
	bool ok = false;
	int const port = configurer.attributeByDevice("gamepad", "port").toInt(&ok);
	if (!ok) {
		throw trikKernel::MalformedConfigException("Incorrect gamepad port");
	}

	init(port);
}

Gamepad::~Gamepad()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}

	qDeleteAll(mButtonStateClearTimers);
}

void Gamepad::reset()
{
	mButtonWasPressed.clear();
	mPads.clear();
}

bool Gamepad::buttonWasPressed(int buttonNumber)
{
	return mButtonWasPressed.remove(buttonNumber);
}

bool Gamepad::buttonIsPressed(int buttonNumber)
{
	return mButtonState[buttonNumber];
}

bool Gamepad::isPadPressed(int pad) const
{
	return mPads.contains(pad) && mPads.value(pad).isPressed;
}

int Gamepad::padX(int pad) const
{
	return (!mPads.contains(pad) || !mPads.value(pad).isPressed) ? -101 : mPads.value(pad).x;
}

int Gamepad::padY(int pad) const
{
	return (!mPads.contains(pad) || !mPads.value(pad).isPressed) ? -101 : mPads.value(pad).y;
}

int Gamepad::wheel() const
{
	return mWheelPercent;
}

bool Gamepad::isConnected() const
{
	return mWorker->activeConnections() > 0;
}

void Gamepad::init(int port)
{
	mWorker.reset(new GamepadServer());

	connect(mWorker.data(), SIGNAL(button(int,int)),this, SLOT(onButton(int,int)));
	connect(mWorker.data(), SIGNAL(pad(int,int,int)),this, SLOT(onPad(int,int,int)));
	connect(mWorker.data(), SIGNAL(padUp(int)),this, SLOT(onPadUp(int)));
	connect(mWorker.data(), SIGNAL(wheel(int)),this, SLOT(onWheel(int)));
	connect(mWorker.data(), SIGNAL(disconnect()),this, SIGNAL(disconnect()));

	mWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();

	QMetaObject::invokeMethod(mWorker.data(), "startServer", Qt::QueuedConnection, Q_ARG(int, port));
}

void Gamepad::onPadUp(int padId)
{
	mPads[padId].isPressed = false;

	emit padUp(padId);
}

void Gamepad::onWheel(int percent)
{
	mWheelPercent = percent;

	emit wheelEvent(percent);
}

void Gamepad::onPad(int padId, int x, int y)
{
	mPads[padId].isPressed = true;
	mPads[padId].x = x;
	mPads[padId].y = y;

	emit pad(padId, x, y);
}

void Gamepad::onButton(int button, int pressed)
{
	if (pressed == 1) {
		mButtonWasPressed.insert(button);
	}

	mButtonState[button] = pressed == 1;
	if (!mButtonStateClearTimers[button]) {
		mButtonStateClearTimers[button] = new QTimer();
		mButtonStateClearTimers[button]->setInterval(500);
		connect(mButtonStateClearTimers[button], SIGNAL(timeout()), this, SLOT(onButtonStateClearTimerTimeout()));
	}

	mButtonStateClearTimers[button]->start();

	emit Gamepad::button(button, pressed);
}

void Gamepad::onButtonStateClearTimerTimeout()
{
	const auto timer = dynamic_cast<QTimer *>(sender());
	if (timer) {
		const int button = mButtonStateClearTimers.key(timer);
		mButtonState[button] = false;
		timer->stop();
	}
}
