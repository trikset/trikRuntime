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

#include "stubIIOFile.h"

#include <QsLog.h>

trikHal::stub::StubIIOFile::StubIIOFile(const QString &fileName, const QString &scanType)
	: mFileName(fileName)
{
	Q_UNUSED(scanType)
}

bool trikHal::stub::StubIIOFile::open()
{
	QLOG_INFO() << "Opening stub:" << mFileName;
	return true;
}

bool trikHal::stub::StubIIOFile::close()
{
	QLOG_INFO() << "Opening stub:" << mFileName;
	return true;
}

QString trikHal::stub::StubIIOFile::fileName() const
{
	return mFileName;
}

bool trikHal::stub::StubIIOFile::isOpened() const
{
	return true;
}
