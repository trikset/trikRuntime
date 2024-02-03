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

#include "translationsHelper.h"

#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QTranslator>
#include <QtCore/QCoreApplication>

#include <QsLog.h>

#include "paths.h"
#include "rcReader.h"

using namespace trikKernel;

void TranslationsHelper::loadTranslators(const QString &locale)
{
	const QDir translationsDirectory(Paths::translationsPath());

	QDirIterator files(translationsDirectory);
	while (files.hasNext()) {
		const QFileInfo &translatorFile = QFileInfo(files.next());
		if (translatorFile.isFile() && translatorFile.baseName().split('_').at(1) == locale) {
			QLOG_INFO() << "Loading translations from" << translatorFile.absolutePath();
			QTranslator *translator = new QTranslator(qApp);
			translator->load(translatorFile.absoluteFilePath());
			QCoreApplication::installTranslator(translator);
		}
	}
}

void TranslationsHelper::initLocale(bool localizationDisabled)
{
	if (localizationDisabled) {
		QLocale::setDefault(QLocale::English);
		return;
	}

	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	QString locale = settings.value("locale", "").toString();
	const QString lastLocale = locale;

	const QFileInfo trikRc(trikKernel::Paths::trikRcName());
	if (locale.isEmpty() && trikRc.exists()) {
		const RcReader rcReader(trikKernel::Paths::trikRcName());
		locale = rcReader.value("locale");
	}

	if (locale.isEmpty()) {
		locale = "ru";
	}

	if (lastLocale != locale) {
		settings.setValue("locale", locale);
	}

	QLocale::setDefault(QLocale(locale));
	loadTranslators(locale);
}
