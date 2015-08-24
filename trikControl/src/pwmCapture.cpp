/* Copyright 2013 - 2015 Roman Kurbatov, Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QtCore/QByteArray>
#include <QtCore/QTextStream>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include <QsLog.h>

using namespace trikControl;

PwmCapture::PwmCapture(const QString &port, const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mFrequencyFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "frequencyFile")))
	, mDutyFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "dutyFile")))
{
	if (!mFrequencyFile->open()) {
		mState.fail();
	}

	if (!mDutyFile->open()) {
		mState.fail();
	}

	mState.ready();
}

PwmCapture::~PwmCapture()
{
}

PwmCapture::Status PwmCapture::status() const
{
	return mState.status();
}

QVector<int> PwmCapture::frequency()
{
	if (!mState.isReady()) {
		return {};
	}

	mFrequencyFile->reset();
	QVector<int> data(3);
	char c = '\0';
	mFrequencyFile->stream() >> data[0] >> c >> data[1] >> c >> data[2] >> c;
	return data;
}

int PwmCapture::duty()
{
	if (!mState.isReady()) {
		return {};
	}

	mDutyFile->reset();
	int data = 0;
	char c = '\0';
	mDutyFile->stream() >> data >> c;
	return data;
}
