/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "stubInputDeviceFile.h"

#include <QsLog.h>

using namespace trikHal::stub;

StubInputDeviceFile::StubInputDeviceFile(const QString &fileName)
	: mFile(fileName)
	, mStream("")
{
}

bool StubInputDeviceFile::open()
{
	QLOG_INFO() << "Opening stub input device file" << mFile.fileName();
	return true;
}

void StubInputDeviceFile::close()
{
	QLOG_INFO() << "Closing stub input device file" << mFile.fileName();
}

QTextStream &StubInputDeviceFile::stream()
{
	return mStream;
}

void StubInputDeviceFile::reset()
{
	QLOG_INFO() << "Resetting stub input device file" << mFile.fileName();
}
