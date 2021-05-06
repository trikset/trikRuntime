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

#include <QtCore/QCoreApplication>
#include <QtCore/QScopedPointer>

#include "commandLineParser.h"

namespace trikKernel {

class LoggingHelper;

/// Helper class for correct initialization of trikRuntime applications --- TrikRun, TrikGui and TrikServer.
/// Usage scenario is to creae application object, create this helper, get command line parser from it and add
/// needed additional command line options, then call "parseCommandLine", then, if it is successful, call "init",
/// then take configPath, if needed.
class TRIKKERNEL_EXPORT ApplicationInitHelper
{
public:
	/// Constructor.
	/// @param app - application object, with filled applicationName field.
	explicit ApplicationInitHelper(QCoreApplication &app);

	~ApplicationInitHelper();

	/// Returns temporary reference to command line options parser.
	trikKernel::CommandLineParser &commandLineParser();

	/// Parses command line arguments.
	/// @returns true, if parsing is successful. All errors will be reported by command line parser itself.
	bool parseCommandLine();

	/// Continues initialization of the application after the command line is parsed.
	void init();

	/// Returns path to a directory with config files, which is possibly received from command line.
	QString configPath() const;

private:
	/// Reference to an application being initialized.
	QCoreApplication &mApp;

	/// Command line arguments parser.
	trikKernel::CommandLineParser mCommandLineParser;

	/// Path to a directory with config files.
	QString mConfigPath;

	/// Initializer for logging system.
	QScopedPointer<LoggingHelper> mLoggingHelper;
};

}
