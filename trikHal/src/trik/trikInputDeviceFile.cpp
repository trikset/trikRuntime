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

#include "trikInputDeviceFile.h"

#include <QsLog.h>

using namespace trikHal::trik;

TrikInputDeviceFile::TrikInputDeviceFile(const QString &fileName)
	: mFile(fileName)
{
}

bool TrikInputDeviceFile::open()
{
	QLOG_INFO() << "Opening input device file" << mFile.fileName();

	if (!mFile.open(QIODevice::ReadOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "File " << mFile.fileName() << " failed to open for reading";
		return false;
	}

	mStream.setDevice(&mFile);

	return true;
}

void TrikInputDeviceFile::close()
{
	QLOG_INFO() << "Closing input device file" << mFile.fileName();
	mFile.close();
}

QTextStream &TrikInputDeviceFile::stream()
{
	return mStream;
}

void TrikInputDeviceFile::reset()
{
	mStream.seek(0);
}
