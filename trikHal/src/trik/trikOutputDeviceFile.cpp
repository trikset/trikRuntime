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

#include "trikOutputDeviceFile.h"

#include <QsLog.h>

using namespace trikHal::trik;

TrikOutputDeviceFile::TrikOutputDeviceFile(const QString &fileName)
	: mFile(fileName)
{
}

bool TrikOutputDeviceFile::open()
{
	if (!mFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "File" << mFile.fileName() << " failed to open for writing";
		return false;
	}

	return true;
}

void TrikOutputDeviceFile::close()
{
	if (mFile.isOpen()) {
		mFile.close();
	}
}

void TrikOutputDeviceFile::write(const QString &data)
{
	mFile.write(data.toUtf8());
	mFile.flush();
}

QString TrikOutputDeviceFile::fileName() const
{
	return mFile.fileName();
}
