/* Copyright 2015 CyberTech Labs Ltd.
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

#include "src/fifo.h"

#include <QtCore/QSocketNotifier>
#include <QtCore/QEventLoop>

#include <trikKernel/configurer.h>


#include "src/configurerHelper.h"

using namespace trikControl;

Fifo::Fifo(const QString &virtualPort, const trikKernel::Configurer &configurer)
	: mFifoFileDescriptor(-1)
{
	Q_UNUSED(virtualPort)
	Q_UNUSED(configurer)

	mState.fail();
}

Fifo::~Fifo()
{
}

DeviceInterface::Status Fifo::status() const
{
	return mState.status();
}

QString Fifo::read()
{
	return "";
}

bool Fifo::hasData() const
{
	return false;
}

void Fifo::readFile()
{
}
