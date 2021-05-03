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

#pragma once

#include <QFile>
#include <QFileInfo>
#include <QsLog.h>

#include "trikRuntimeException.h"

namespace trikKernel {

/// Exception that is thrown when file opening operation failed.
class TRIKKERNEL_EXPORT FailedToOpenFileException : public TrikRuntimeException
{
public:
	/// Constructor.
	/// @param file - file that is failed to open.
	explicit FailedToOpenFileException(const QFile &file)
		: TrikRuntimeException(QString("Failed to open %1 %2")
							   .arg(QFileInfo(file).absoluteFilePath())
							   .arg((file.openMode() & QIODevice::WriteOnly ? "for writing" : "for reading")))
		  , mFile(file)
	{
	}

	/// Returns file that is failed to open.
	const QFile &file() const
	{
		return mFile;
	}

private:
	const QFile &mFile;
};

}
