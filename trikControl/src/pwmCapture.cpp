/* Copyright 2013 Roman Kurbatov
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

#include "pwmCapture.h"

#include <QtCore/QDebug>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>

using namespace trikControl;

PwmCapture::PwmCapture(QString const &periodFile, QString const &dutyFile)
	: mPeriodFile(periodFile)
	, mDutyFile(dutyFile)
{
	if (!mPeriodFile.open(QIODevice::ReadOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		qDebug() << "Can't open period capture file " << mPeriodFile.fileName();
	}

	if (!mDutyFile.open(QIODevice::ReadOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		qDebug() << "Can't open duty capture file " << mDutyFile.fileName();
	}
}

PwmCapture::~PwmCapture()
{
	mPeriodFile.close();
	mDutyFile.close();
}

QVector<int> PwmCapture::period()
{
	mPeriodFile.reset();
	QByteArray dataText = mPeriodFile.readAll();
	QTextStream stream(dataText);
	QVector<int> data(3);
	char c;
	stream >> data[0] >> c >> data[1] >> c >> data[2] >> c;
	return data;
}

int PwmCapture::duty()
{
	mDutyFile.reset();
	QByteArray dataText = mDutyFile.readAll();
	QTextStream stream(dataText);
	int data = 0;
	char c;
	stream >> data >> c;
	return data;
}
