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

#include <QAbstractListModel>
#include <QQmlApplicationEngine>

namespace trikGui {

/// Widget that allows selection of TrikGUI interface language. Uses "translations" folder and "locale.ini" files
/// there to find available languages. Stores selected language in localSettings.ini.
class LanguageSelection : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(QVector<QString> availableLocales READ availableLocales CONSTANT)

public:
	/// Constructor
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit LanguageSelection(QQmlApplicationEngine *engine, QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	Q_INVOKABLE void switchLanguage(QString targetLanguage);
	Q_INVOKABLE void setQmlParent(QObject *parent);

private:
	/// Loads available locales from "translations" folder.
	void loadLocales();
	QQmlApplicationEngine *mEngine;
	QVector<QString> mAvailableLocales;
	QVector<QString> availableLocales();
};

}
