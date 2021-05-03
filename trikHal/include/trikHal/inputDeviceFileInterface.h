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

#pragma once

#include <QtCore/QTextStream>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Input device file abstraction. Can read from a device file, thus communicating with a device driver.
class TRIKHAL_EXPORT InputDeviceFileInterface
{
	Q_DISABLE_COPY(InputDeviceFileInterface)
public:
	InputDeviceFileInterface() = default;
	virtual ~InputDeviceFileInterface() = default;

	/// Open a file. File name must be set previously.
	virtual bool open() = 0;

	/// Close a file.
	virtual void close() = 0;

	/// Returns temporary reference to a stream which can be used to read from a file.
	virtual QTextStream &stream() = 0;

	/// Resets input file, moving file cursor to the beginning of the file.
	virtual void reset() = 0;
};

}
