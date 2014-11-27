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

#include "gamepad.h"

#include <QtCore/QStringList>

#include "tcpConnector.h"

#include "QsLog.h"

using namespace trikControl;

Gamepad::Gamepad(int port)
	: mListener(new TcpConnector(port))
{
	connect(mListener.data(), SIGNAL(dataReady(QString)), this, SLOT(parse(QString)));
	connect(&mNetworkThread, SIGNAL(started()), mListener.data(), SLOT(startServer()));
	connect(mListener.data(), SIGNAL(tcpDisconnectedSignal()), this, SIGNAL(disconnect()));
	mListener->moveToThread(&mNetworkThread);
	mNetworkThread.start();
}

Gamepad::~Gamepad()
{
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

bool Gamepad::isPadPressed(int pad)
{
	return mPads.contains(pad) && mPads.value(pad).isPressed;
}

int Gamepad::padX(int pad)
{
	return (!mPads.contains(pad) || !mPads.value(pad).isPressed) ? -1 : mPads.value(pad).x;
}

int Gamepad::padY(int pad)
{
	return (!mPads.contains(pad) || !mPads.value(pad).isPressed) ? -1 : mPads.value(pad).y;
}

void Gamepad::parse(QString const &message)
{
	QStringList const cmd = message.split(" ", QString::SkipEmptyParts);
	QString const commandName = cmd.at(0).trimmed();
	if (commandName == "pad") {
		int const padId = cmd.at(1).trimmed().toInt();
		if (cmd.at(2).trimmed() == "up") {
			mPads[padId].isPressed = false;
			emit padUp(padId);
		} else {
			int const x = cmd.at(2).trimmed().toInt();
			int const y = cmd.at(3).trimmed().toInt();
			mPads[padId].isPressed = true;
			mPads[padId].x = x;
			mPads[padId].y = y;
			emit pad(padId, x, y);
		}
	} else if (commandName == "btn") {
		int const buttonCode = cmd.at(1).trimmed().toInt();
		mButtonWasPressed.insert(buttonCode);
		emit button(buttonCode, 1);
	} else if (commandName == "wheel") {
		int const perc = cmd.at(1).trimmed().toInt();
		emit wheel(perc);
	} else {
		QLOG_ERROR() << "Gamepad: unknown command" << commandName;
		qDebug() << "Gamepad: unknown command" << commandName;
	}
}
