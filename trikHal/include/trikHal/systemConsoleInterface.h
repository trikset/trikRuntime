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

#include <QtCore/QStringList>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Represents console of an OS.
class TRIKHAL_EXPORT SystemConsoleInterface
{
	Q_DISABLE_COPY(SystemConsoleInterface)
public:
	virtual ~SystemConsoleInterface() = default;

	SystemConsoleInterface() = default;

	/// Executes given command on a system console. Returns return code of a command.
	virtual int system(const QString &command) = 0;

	/// Asynchronously starts given process with given arguments.
	/// @returns true, if process was started successfully.
	virtual bool startProcess(const QString &processName, const QStringList &arguments) = 0;

	/// Synchronously starts given process with given arguments.
	/// @returns true, if process was started successfully.
	virtual bool startProcessSynchronously(const QString &processName, const QStringList &arguments
			, QString * output = nullptr) = 0;
};

}
