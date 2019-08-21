/* Copyright 2013 - 2016 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QsLog.h>

using namespace trikControl;

Gamepad::Gamepad(const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mUnderlyingFifo(configurer.attributeByDevice("gamepad", "file"), hardwareAbstraction)
{
	mKeepaliveTimer.setSingleShot(true);
	connect(&mKeepaliveTimer, &QTimer::timeout, this, &Gamepad::disconnect);
	connect(&mUnderlyingFifo, &Fifo::newData, this, &Gamepad::onNewData);
}

void Gamepad::reset()
{
	mButtonWasPressed.clear();
	mPads.clear();
}

Gamepad::Status Gamepad::status() const
{
	return mUnderlyingFifo.status();
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
	return mConnected && status() == Status::ready;
}

bool Gamepad::disconnect()
{
	auto wasConnected = isConnected();
	if (wasConnected) {
		mConnected = false;
		reset();
		emit disconnected();
	}
	return wasConnected;
}

void Gamepad::onNewData(const QString &data)
{
	if (!isConnected()) {
		mConnected = true;
		emit connected();
	}

	const QStringList cmd = data.split(" ", QString::SkipEmptyParts);
	if (cmd.isEmpty()) {
		return;
	}

	const QString commandName = cmd.at(0).trimmed();
	if (commandName == "pad") {
		if (cmd.length() < 3) {
			QLOG_ERROR() << "Malformed 'pad' command '" << data << "'. Ignoring.";
			return;
		}

		const int padId = cmd.at(1).trimmed().toInt();
		if (cmd.at(2).trimmed() == "up") {
			handlePadUp(padId);
		} else {
			if (cmd.length() < 4) {
				QLOG_ERROR() << "Malformed 'pad' command '" << data << "'. Ignoring.";
				return;
			}

			const int x = cmd.at(2).trimmed().toInt();
			const int y = cmd.at(3).trimmed().toInt();
			handlePad(padId, x, y);
		}
	} else if (commandName == "btn") {
		if (cmd.length() < 2) {
			QLOG_ERROR() << "Malformed 'btn' command '" << data << "'. Ignoring.";
			return;
		}

		const int buttonCode = cmd.at(1).trimmed().toInt();
		int state = 1;
		if (cmd.length() > 2) {
			state = cmd.at(2) == "down" ? 1 : 0;
		}

		handleButton(buttonCode, state);
	} else if (commandName == "wheel") {
		const int perc = cmd.at(1).trimmed().toInt();
		handleWheel(perc);
	} else if (commandName == "keepalive") {
		const int waitForMs = cmd.at(1).trimmed().toInt();
		handleKeepalive(waitForMs);
	} else {
		QLOG_ERROR() << "Gamepad: unknown command" << commandName;
	}
}

void Gamepad::handlePadUp(int padId)
{
	mPads[padId].isPressed = false;
	emit padUp(padId);
}

void Gamepad::handleWheel(int percent)
{
	mWheelPercent = percent;
	emit wheelEvent(percent);
}

void Gamepad::handlePad(int padId, int x, int y)
{
	mPads[padId].isPressed = true;
	mPads[padId].x = x;
	mPads[padId].y = y;
	emit pad(padId, x, y);
}

void Gamepad::handleButton(int button, int pressed)
{
	if (pressed == 1) {
		mButtonWasPressed.insert(button);
	}

	mButtonState[button] = pressed == 1;
	auto & tmr = mButtonStateClearTimers[button];
	if (!tmr) {
		tmr = new QTimer(this);
		tmr->setInterval(500);
		connect(
				tmr
				, SIGNAL(timeout())
				, this
				, SLOT(onButtonStateClearTimerTimeout())
				);
	}

	tmr->start();

	emit Gamepad::button(button, pressed);
}

void Gamepad::handleKeepalive(int waitForMs)
{
	if (waitForMs == 0) {
		mKeepaliveTimer.stop();
	} else {
		mKeepaliveTimer.start(waitForMs);
	}
}

void Gamepad::onButtonStateClearTimerTimeout()
{
	const auto timer = qobject_cast<QTimer *>(sender());
	if (timer) {
		const int button = mButtonStateClearTimers.key(timer);
		mButtonState[button] = false;
		timer->stop();
	}
}
