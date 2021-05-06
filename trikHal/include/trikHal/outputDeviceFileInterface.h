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

#include <QtCore/QString>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Output device file abstraction. Can only write to a device file, thus sending commands to a device driver.
/// Flushes its contents after every write.
class TRIKHAL_EXPORT OutputDeviceFileInterface
{
	Q_DISABLE_COPY(OutputDeviceFileInterface)
public:
	OutputDeviceFileInterface() = default;
	virtual ~OutputDeviceFileInterface() = default;

	/// Open a file. File name must be set previously.
	virtual bool open() = 0;

	/// Close a file.
	virtual void close() = 0;

	/// Write data to a file using UTF-8 encoding.
	virtual void write(const QString &data) = 0;

	/// Returns name of a file.
	virtual QString fileName() const = 0;
};

}
