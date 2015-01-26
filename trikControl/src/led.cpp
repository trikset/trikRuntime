/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include <trikKernel/configurer.h>

#include <QsLog.h>

using namespace trikControl;

Led::Led(trikKernel::Configurer const &configurer)
	: mRedDeviceFile(configurer.attributeByDevice("led", "red"))
	, mGreenDeviceFile(configurer.attributeByDevice("led", "green"))
{
	if (!mRedDeviceFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open red led control file " << mRedDeviceFile.fileName();
		mState.fail();
	}

	if (!mGreenDeviceFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open green led control file " << mGreenDeviceFile.fileName();
		mState.fail();
	}

	mState.ready();
}

Led::~Led()
{
	red();

	mRedDeviceFile.close();
	mGreenDeviceFile.close();
}

Led::Status Led::status() const
{
	return mState.status();
}

void Led::red()
{
	if (mState.isReady()) {
		mRedDeviceFile.write("1");
		mRedDeviceFile.flush();

		mGreenDeviceFile.write("0");
		mGreenDeviceFile.flush();
	}
}

void Led::green()
{
	if (mState.isReady()) {
		mRedDeviceFile.write("0");
		mRedDeviceFile.flush();

		mGreenDeviceFile.write("1");
		mGreenDeviceFile.flush();
	}
}

void Led::orange()
{
	if (mState.isReady()) {
		mRedDeviceFile.write("1");
		mRedDeviceFile.flush();

		mGreenDeviceFile.write("1");
		mGreenDeviceFile.flush();
	}
}

void Led::off()
{
	if (mState.isReady()) {
		mRedDeviceFile.write("0");
		mRedDeviceFile.flush();

		mGreenDeviceFile.write("0");
		mGreenDeviceFile.flush();
	}
}
