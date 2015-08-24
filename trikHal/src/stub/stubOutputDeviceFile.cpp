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

#include "stubOutputDeviceFile.h"

#include <QsLog.h>

using namespace trikHal::stub;

StubOutputDeviceFile::StubOutputDeviceFile(const QString &fileName)
	: mFile(fileName)
{
}

bool StubOutputDeviceFile::open()
{
	QLOG_INFO() << "Opening stub output device file" << mFile.fileName();
	return true;
}

void StubOutputDeviceFile::close()
{
	QLOG_INFO() << "Closing stub output device file" << mFile.fileName();
}

void StubOutputDeviceFile::write(const QString &data)
{
	QLOG_INFO() << "Writing to stub output device file" << mFile.fileName() << ":" << data;
}

QString StubOutputDeviceFile::fileName() const
{
	return mFile.fileName();
}
