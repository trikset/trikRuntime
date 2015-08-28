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

namespace trikKernel {

/// Parser of command-line parameters.
class CommandLineParser
{
public:
	/// Add application description string.
	void addApplicationDescription(const QString &description);
	void addPositionalArgument(const QString &name, const QString &description);
	void addOption(const QString &shortName, const QString &longName, const QString &description);
	void addFlag(const QString &shortName, const QString &longName, const QString &description);

	bool process(const QCoreApplication &app);
	void showHelp() const;

	QStringList positionalArgs() const;
	bool isSet(const QString &optionShortName) const;
	QString value(const QString optionShortName) const;

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
