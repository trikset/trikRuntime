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

#include "src/rangeSensorWorker.h"

using namespace trikControl;

RangeSensorWorker::RangeSensorWorker(QString const &eventFile)
	: mEventFile(eventFile)
{
}

RangeSensorWorker::~RangeSensorWorker()
{
}

void RangeSensorWorker::stop()
{
}

void RangeSensorWorker::init()
{
}

void RangeSensorWorker::readFile()
{
}

int RangeSensorWorker::read()
{
	return 0;
}

int RangeSensorWorker::readRawData()
{
	return 0;
}
