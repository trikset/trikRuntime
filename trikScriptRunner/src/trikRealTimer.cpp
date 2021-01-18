/* Copyright 2021 CyberTech Labs Ltd.
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

#include "trikRealTimer.h"

using namespace trikScriptRunner;

TrikRealTimer::TrikRealTimer()
{
	setRepeatable(true);
	QObject::connect(&mTimer, &QTimer::timeout, this, &TrikRealTimer::timeout);
}

bool TrikRealTimer::isTicking() const
{
	return mTimer.isActive();
}

int TrikRealTimer::interval() const
{
	return mTimer.interval();
}

void TrikRealTimer::start()
{
	start(mTimer.interval());
}

void TrikRealTimer::start(int ms)
{
	mTimer.setInterval(ms);
	mTimer.start();
}

void TrikRealTimer::stop()
{
	mTimer.stop();
}

void TrikRealTimer::setInterval(int ms)
{
	mTimer.setInterval(ms);
}

void TrikRealTimer::setRepeatable(bool repeatable)
{
	mTimer.setSingleShot(!repeatable);
}
