/* Copyright 2026 CyberTech Labs Ltd.
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

#include "stubFbOutput.h"

#include <QsLog.h>

using namespace trikHal;
using namespace trikHal::stub;

StubFbOutput::StubFbOutput(QObject *parent)
	: FbOutputInterface(parent)
{
}

bool StubFbOutput::open()
{
	mOpen = true;
	return true;
}

void StubFbOutput::close()
{
	if (!mOpen) return;
	mOpen = false;
}

bool StubFbOutput::isOpen() const { return mOpen; }

void StubFbOutput::writeFrame(const uint8_t *) { /* no-op */ }

uint32_t StubFbOutput::frameWidth() const  { return 240; }
uint32_t StubFbOutput::frameHeight() const { return 240; }
