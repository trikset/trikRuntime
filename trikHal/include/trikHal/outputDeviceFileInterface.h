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

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Output device file abstraction. Can only write to a device file, thus sending commands to a device driver.
/// Flushes its contents after every write.
class TRIKHAL_EXPORT OutputDeviceFileInterface
{
	Q_DISABLE_COPY(OutputDeviceFileInterface)
public:
	/// How the file is opened. Determines the write semantics.
	enum class OpenMode {
		/// Text command write to a device file (the historic behaviour): the file
		/// is opened write-only and @c write(QString) is used. Opening may block.
		Text,

		/// Raw non-blocking binary write, intended for streaming frames (e.g. JPEG)
		/// into a Linux FIFO. The FIFO node is created on demand, the descriptor is
		/// opened read-write + non-blocking, and @c write(QByteArray) drops data
		/// instead of blocking when the pipe is full.
		NonBlockingBinary,
	};

	OutputDeviceFileInterface() = default;
	virtual ~OutputDeviceFileInterface() = default;

	/// Open a file. File name must be set previously.
	virtual bool open(OpenMode mode = OpenMode::Text) = 0; // NOLINT(google-default-arguments)

	/// Close a file.
	virtual void close() = 0;

	/// Write data to a file using UTF-8 encoding.
	virtual void write(const QString &data) = 0;

	/// Write raw bytes to a file. Non-blocking in @c NonBlockingBinary mode.
	/// @returns true if the whole buffer was written (false when dropped).
	virtual bool write(const QByteArray &data) = 0;

	/// Returns name of a file.
	virtual QString fileName() const = 0;
};

}
