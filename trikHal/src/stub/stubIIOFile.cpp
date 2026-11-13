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

#include <utility>

#include <QsLog.h>

trikHal::stub::StubIIOFile::StubIIOFile(QString fileName)
	: mFileName(std::move(fileName))
{
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

QVector<trikHal::IIOChannelInfo> trikHal::stub::StubIIOFile::channels() const
{
	return {};
}

int trikHal::stub::StubIIOFile::channelValueIndex(trikHal::IIOChanType type, trikHal::IIOAxis axis) const
{
	Q_UNUSED(type)
	Q_UNUSED(axis)
	return -1;
}

int trikHal::stub::StubIIOFile::recordBytes() const
{
	return 0;
}

bool trikHal::stub::StubIIOFile::hasTimestamp() const
{
	return false;
}
