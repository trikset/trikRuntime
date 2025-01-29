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
#include <memory>
#include <trikKernel/trikKernelDeclSpec.h>
#include <QsLogLevel.h>
#include <QsLogDest.h>

namespace trikKernel {

/// Helper for working with QsLog library.
class TRIKKERNEL_EXPORT LoggingHelper
{
public:
	/// Constructor. Initializes logger with default settings, supposed to be alive until program finishes (RAII idiom).
	/// @param pathToLog - path to "trik.log" file that will be created or appended by logger. Supposed to end with "/".
	LoggingHelper(const QString &pathToLog, QsLogging::Level consoleLogLevel);

	~LoggingHelper();

private:
	std::unique_ptr<QsLogging::Destination> mFileDestination;
	std::unique_ptr<QsLogging::Destination> mConsoleDestination;
};

}
