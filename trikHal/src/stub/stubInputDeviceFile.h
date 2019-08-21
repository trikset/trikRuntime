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

#include <QtCore/QString>
#include <QtCore/QFile>

#include "inputDeviceFileInterface.h"

namespace trikHal {
namespace stub {

/// Empty implementation of input device file. Only logs operations, returns empty stream.
class StubInputDeviceFile : public InputDeviceFileInterface
{
public:
	/// Constructor.
	/// @param fileName - file name (with path, relative or absolute) of a device file.
	explicit StubInputDeviceFile(const QString &fileName);

	bool open() override;
	void close() override;
	QTextStream &stream() override;
	void reset() override;

private:
	QFile mFile;
	QTextStream mStream;
};

}
}
