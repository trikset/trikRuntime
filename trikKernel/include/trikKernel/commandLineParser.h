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
#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include "trikKernelDeclSpec.h"

namespace trikKernel {

/// Parser of command-line parameters. Here "positional argument" is an argument that must be passed to an application
/// in some order, "option" is an argument that must appear in command line in arbitrary order (after last positional
/// argument) and must be preceded by its key (short or long form), and flag is like option, but does not have value
/// and can only be set (when it appears in command line) and unset (when it is omitted).
/// For example, "./trikRun file.js --config ./configs --no-locale" has one positional argument ("file.js"),
/// one option ("--config" with value "./configs") and one flag ("--no-locale).
/// Options and flags in short form are detected by "-" prefix, and in long form --- by "--", so if some positional
/// argument starts with "-" it must be quoted.
class TRIKKERNEL_EXPORT CommandLineParser
{
public:
	/// Adds application description string.
	void addApplicationDescription(const QString &description);

	/// Adds new positional argument with its description.
	void addPositionalArgument(const QString &name, const QString &description);

	/// Add new option with short and long forms and descriptions. Forms must be passed without "-" prefixes.
	/// For example, addOption("c", "config", tr("Path to a config file"));
	void addOption(const QString &shortName, const QString &longName, const QString &description);

	/// Add new flag with short and long forms and descriptions. Forms must be passed without "-" prefixes.
	/// For example, addFlag("h", "help", tr("Print this help message"));
	void addFlag(const QString &shortName, const QString &longName, const QString &description);

	/// Parse command line arguments of given application.
	/// @returns true if parsing is successful. Does not report errors.
	bool process(const QCoreApplication &app);

	/// Shows application info and descriptions of all command line arguments.
	void showHelp() const;

	/// Returns a list of positional arguments in order in which they appeared in command line.
	QStringList positionalArgs() const;

	/// Returns true, if option or flag appeared in command line.
	bool isSet(const QString &optionShortName) const;

	/// Returns value of an option or an empty string if it is not set.
	QString value(const QString &optionShortName) const;

private:
	QString mApplicationName;
	QString mApplicationVersion;
	QString mApplicationDescription;
	QStringList mPositionalArgNames;
	QStringList mPositionalArgDescriptions;
	QStringList mPositionalArgValues;
	QHash<QString, QString> mOptionDescriptions;
	QHash<QString, QString> mOptionLongNames;
	QHash<QString, QString> mOptionValues;
	QHash<QString, QString> mFlagDescriptions;
	QHash<QString, QString> mFlagLongNames;
	QSet<QString> mSetFlags;
};

}
