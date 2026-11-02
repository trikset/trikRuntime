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

#include <gtest/gtest.h>

#include <QtCore/qglobal.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include <trikKernel/coreDumping.h>
#include <trikKernel/loggingHelper.h>

#include <testUtils/eventFilter.h>

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	QCoreApplication app(argc, argv);

	QFile srf04Distance("test-srf04-distance.txt");
	if (srf04Distance.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		srf04Distance.write("0\n");
		srf04Distance.close();
	}

	tests::utils::EventFilter eventFilter;
	// useful to debug events
	// app.installEventFilter(&eventFilter);

	trikKernel::LoggingHelper loggingHelper(".", QsLogging::Level::WarnLevel);
	Q_UNUSED(loggingHelper);

	return RUN_ALL_TESTS();
}
