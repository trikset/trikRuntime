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

#include "loggingHelper.h"
#include <iostream>
#include <iomanip>
#include <QtCore/QDir>

#include <QsLog.h>
#include <QsLogDestConsole.h>

using namespace trikKernel;

LoggingHelper::LoggingHelper(const QString &pathToLog)
{
	const QString correctedPath = pathToLog.endsWith(QDir::separator())
			? pathToLog
			: pathToLog + QDir::separator();

	QDir logDir(correctedPath);
	if (!logDir.exists()) {
		logDir.mkpath(correctedPath);
	}

	const int maxLogSize = 10 * 1024 * 1024;
	QsLogging::Logger::instance().setLoggingLevel(QsLogging::TraceLevel);
	QsLogging::Logger::instance().addDestination(QsLogging::DestinationFactory::MakeFileDestination(
			correctedPath + "trik.log"
			, QsLogging::LogRotationOption::EnableLogRotation
			, QsLogging::MaxSizeBytes(maxLogSize)
			, QsLogging::MaxOldLogCount(2)));

	QsLogging::Logger::instance().addDestination(QsLogging::DestinationFactory::MakeFunctorDestination(
			[](const QsLogging::LogMessage &message) {
				if (message.level >= QsLogging::ErrorLevel) {
					std::cerr << qPrintable(message.formatted) << std::endl << std::flush;
				}
			}
			));
}

LoggingHelper::~LoggingHelper()
{
#if defined(Q_OS_WIN)
	QsLogging::Logger::instance().shutDownLoggerThread();
#endif
}
