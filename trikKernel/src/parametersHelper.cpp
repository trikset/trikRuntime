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

#include "parametersHelper.h"

#include <QtCore/QDebug>

using namespace trikKernel;

void ParametersHelper::addApplicationDescription(const QString &description)
{
	mApplicationDescription = description;
}

void ParametersHelper::addPositionalArgument(const QString &name, const QString &description)
{
	mPositionalArgNames << name;
	mPositionalArgDescriptions << description;
}

void ParametersHelper::addOption(const QString &shortName, const QString &longName, const QString &description)
{
	mOptionDescriptions.insert(shortName, description);
	mOptionLongNames.insert(longName, shortName);
}

void ParametersHelper::addFlag(const QString &shortName, const QString &longName, const QString &description)
{
	mFlagDescriptions.insert(shortName, description);
	mFlagLongNames.insert(longName, shortName);
}

bool ParametersHelper::process(const QCoreApplication &app)
{
	mApplicationName = app.applicationName();
	mApplicationVersion = app.applicationVersion();

	const QStringList args = app.arguments();

	int currentArgCounter = 0;
	for (const QString &arg : args) {
		if (arg.startsWith('-')) {
			break;
		}

		mPositionalArgValues << arg;
		++currentArgCounter;
	}

	for (int i = currentArgCounter; i < args.count(); ++i) {
		QString currentArg = args[i];
		if (currentArg.startsWith("--")) {
			currentArg = currentArg.mid(2);
			if (mOptionLongNames.contains(currentArg)) {
				currentArg = mOptionLongNames[currentArg];
			} else if (mFlagLongNames.contains(currentArg)) {
				currentArg = mFlagLongNames[currentArg];
			} else {
				// Argument looks like option long name but it was not registered.
				return false;
			}
		} else if (currentArg.startsWith('-')) {
			currentArg = currentArg.mid(1);
		} else {
			// Argument has no '-' sign, so it looks like value, but we do not expect value here.
			return false;
		}

		if (mOptionDescriptions.contains(currentArg)) {
			if (i + 1 >= args.count()) {
				// There is optional argument but no option value.
				return false;
			}

			const QString optionValue = args[i + 1];
			mOptionValues.insert(currentArg, optionValue);
			++i;
		} else if (mFlagDescriptions.contains(currentArg)) {
			mSetFlags.insert(currentArg);
		} else {
			// Unknown argument.
			return false;
		}
	}

	return true;
}

void ParametersHelper::showHelp() const
{
	const QString generalInfo = mApplicationName
			+ (mApplicationVersion.isEmpty() ? "" : (" (v " + mApplicationVersion + ")"));

	qDebug() << generalInfo;

	if (!mPositionalArgNames.isEmpty()) {
		qDebug() << "Positional arguments:";
		for (int i = 0; i < mPositionalArgNames.size(); ++i) {
			qDebug() << mPositionalArgNames[i] << ": " << mPositionalArgDescriptions[i];
		}
	}

	const auto printInfo = [](auto &&descriptions, auto &&longNames, auto &&help) {
		if (!descriptions.isEmpty()) {
			qDebug() << help << ":";
			for (const QString &optionShortName : descriptions.keys()) {
				const QString option = "-" + optionShortName
						+ (longNames.value(optionShortName).isEmpty()
								? ""
								: "(--" + longNames[optionShortName] + ")");

				qDebug() << option << ": " << descriptions[optionShortName];
			}
		}
	};

	printInfo(mOptionDescriptions, mOptionLongNames, "Options");
	printInfo(mFlagDescriptions, mFlagLongNames, "Flags");
}

QStringList ParametersHelper::positionalArgs() const
{
	return mPositionalArgValues;
}

bool ParametersHelper::isSet(const QString &optionShortName) const
{
	return mOptionValues.contains(optionShortName) || mSetFlags.contains(optionShortName);
}

QString ParametersHelper::value(const QString optionShortName) const
{
	return mOptionValues.value(optionShortName);
}
