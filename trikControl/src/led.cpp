/* Copyright 2013 Yurii Litvinov
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

#include "led.h"

#include <QtCore/QDebug>

#include "QsLog.h"

using namespace trikControl;

Led::Led(QString const &redDeviceFile, QString const &greenDeviceFile, int on, int off)
	: mRedDeviceFile(redDeviceFile)
	, mGreenDeviceFile(greenDeviceFile)
	, mOn(on)
	, mOff(off)
{
	if (!mRedDeviceFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open red led control file " << mRedDeviceFile.fileName();
		qDebug() << "Can't open red led control file " << mRedDeviceFile.fileName();
	}

	if (!mGreenDeviceFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open green led control file " << mGreenDeviceFile.fileName();
		qDebug() << "Can't open green led control file " << mGreenDeviceFile.fileName();
	}
}

Led::~Led()
{
	red();

	mRedDeviceFile.close();
	mGreenDeviceFile.close();
}

void Led::red()
{
	off();

	QString const command = QString::number(mOn);

	mRedDeviceFile.write(command.toLatin1());
	mRedDeviceFile.flush();
}

void Led::green()
{
	off();

	QString const command = QString::number(mOn);

	mGreenDeviceFile.write(command.toLatin1());
	mGreenDeviceFile.flush();
}

void Led::orange()
{
	QString const command = QString::number(mOn);

	mRedDeviceFile.write(command.toLatin1());
	mRedDeviceFile.flush();

	mGreenDeviceFile.write(command.toLatin1());
	mGreenDeviceFile.flush();
}

void Led::off()
{
	QString const command = QString::number(mOff);

	mRedDeviceFile.write(command.toLatin1());
	mRedDeviceFile.flush();

	mGreenDeviceFile.write(command.toLatin1());
	mGreenDeviceFile.flush();
}
