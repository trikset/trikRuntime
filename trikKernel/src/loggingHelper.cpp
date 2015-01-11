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

#include <QsLog.h>

using namespace trikKernel;

LoggingHelper::LoggingHelper(QString const &pathToLog)
{
	int const maxLogSize = 10 * 1024 * 1024;
	QsLogging::Logger::instance().setLoggingLevel(QsLogging::TraceLevel);
	mFileDestination = QsLogging::DestinationFactory::MakeFileDestination(
			pathToLog + "trik.log"
			, QsLogging::EnableLogRotation
			, QsLogging::MaxSizeBytes(maxLogSize)
			, QsLogging::MaxOldLogCount(2)
			, QsLogging::TraceLevel);

	mConsoleDestination = QsLogging::DestinationFactory::MakeFunctorDestination(
			[](QString const &message, QsLogging::Level level) {
				if (level >= QsLogging::ErrorLevel) {
					qDebug() << message;
				}
			}
			);

	QsLogging::Logger::instance().addDestination(mFileDestination);
	QsLogging::Logger::instance().addDestination(mConsoleDestination);
}

LoggingHelper::~LoggingHelper()
{
}
