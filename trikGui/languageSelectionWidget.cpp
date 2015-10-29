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

#include "languageSelectionWidget.h"

#include <QsLog.h>

#include <QtCore/QSettings>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QApplication>
	#include <QtGui/QMessageBox>
#else
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QMessageBox>
#endif

#include <QtGui/QKeyEvent>

#include <trikKernel/paths.h>

using namespace trikGui;

LanguageSelectionWidget::LanguageSelectionWidget(QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("Select language:"))
{
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	const QString lastLocale = settings.value("locale", "").toString();

	int lastLocaleIndex = 0;
	const auto english = new QListWidgetItem(tr("English"));
	english->setData(Qt::UserRole, "en");
	mLanguages.addItem(english);

	loadLocales();

	int i = 0;
	for (const QString &locale : mAvailableLocales.keys()) {
		++i;
		const auto localeItem = new QListWidgetItem(mAvailableLocales.value(locale));
		localeItem->setData(Qt::UserRole, locale);
		mLanguages.addItem(localeItem);
		if (locale == lastLocale) {
			lastLocaleIndex = i;
		}
	}

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mLanguages);
	setLayout(&mLayout);

	mLanguages.selectionModel()->select(
			mLanguages.model()->index(lastLocaleIndex, 0)
			, QItemSelectionModel::ClearAndSelect
			);

	mLanguages.setCurrentIndex(mLanguages.model()->index(lastLocaleIndex, 0));
}

QString LanguageSelectionWidget::menuEntry()
{
	return tr("Language");
}

void LanguageSelectionWidget::renewFocus()
{
	mLanguages.setFocus();
}

void LanguageSelectionWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return: {
			const QString selectedLocale = mLanguages.currentItem()->data(Qt::UserRole).toString();
			QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
			settings.setValue("locale", selectedLocale);
			settings.sync();

			QMessageBox restartMessageBox(QMessageBox::Warning, tr("Warning")
					, tr("GUI will now restart"), QMessageBox::Ok);

			restartMessageBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
			restartMessageBox.exec();
			QApplication::exit(0);
			break;
		}
		default: {
			TrikGuiDialog::keyPressEvent(event);
			break;
		}
	}
}

void LanguageSelectionWidget::loadLocales()
{
	const QDir translationsDirectory(trikKernel::Paths::translationsPath());
	QDirIterator directories(translationsDirectory);
	while (directories.hasNext()) {
		const QFileInfo localeInfo(directories.next() + "/locale.ini");
		if (localeInfo.exists()) {
			QSettings parsedLocaleInfo(localeInfo.absoluteFilePath(), QSettings::IniFormat);
			parsedLocaleInfo.setIniCodec("UTF-8");
			parsedLocaleInfo.sync();
			const QString localeName = parsedLocaleInfo.value("name", "").toString();
			if (localeName != "") {
				mAvailableLocales.insert(localeInfo.dir().dirName(), localeName);
			}
		}
	}
}
