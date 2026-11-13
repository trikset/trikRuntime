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

namespace {

QVector<trikHal::IIOChannelInfo> stubChannels()
{
	auto format = trikHal::IIOChannelFormat {};
	format.storageBits = 16;
	format.bigEndian = false;
	format.isSigned = true;
	format.shift = 0;

	QVector<trikHal::IIOChannelInfo> result;
	const auto add = [&result, &format](trikHal::IIOChanType type, trikHal::IIOAxis axis, int index) {
		auto info = trikHal::IIOChannelInfo {};
		info.id.type = type;
		info.id.axis = axis;
		info.index = index;
		info.format = format;
		result.append(info);
	};

	add(trikHal::IIOChanType::Accel, trikHal::IIOAxis::X, 0);
	add(trikHal::IIOChanType::Accel, trikHal::IIOAxis::Y, 1);
	add(trikHal::IIOChanType::Accel, trikHal::IIOAxis::Z, 2);
	add(trikHal::IIOChanType::AnglVel, trikHal::IIOAxis::X, 3);
	add(trikHal::IIOChanType::AnglVel, trikHal::IIOAxis::Y, 4);
	add(trikHal::IIOChanType::AnglVel, trikHal::IIOAxis::Z, 5);
	add(trikHal::IIOChanType::Temp, trikHal::IIOAxis::None, 6);

	return result;
}

}

QVector<trikHal::IIOChannelInfo> trikHal::stub::StubIIOFile::channels() const
{
	return stubChannels();
}

int trikHal::stub::StubIIOFile::channelValueIndex(trikHal::IIOChanType type, trikHal::IIOAxis axis) const
{
	const auto channels = stubChannels();
	for (auto i = 0; i < channels.size(); ++i) {
		if (channels.at(i).id.type == type && channels.at(i).id.axis == axis) {
			return i;
		}
	}

	return -1;
}

int trikHal::stub::StubIIOFile::recordBytes() const
{
	return 16;
}

bool trikHal::stub::StubIIOFile::hasTimestamp() const
{
	return true;
}
