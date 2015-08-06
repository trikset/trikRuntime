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

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QListWidget>
#else
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListWidget>
#endif

#include <QtCore/QString>
#include <QtCore/QMap>

#include "trikGuiDialog.h"

namespace trikGui {

/// Widget that allows selection of TrikGUI interface language. Uses "translations" folder and "locale.ini" files
/// there to find available languages. Stores selected language in localSettings.ini.
class LanguageSelectionWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param parent - parent of this widget in Qt object hierarchy.
	LanguageSelectionWidget(QWidget *parent = 0);

	/// Returns menu entry for this widget.
	static QString menuEntry();

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	/// Loads available locales from "translations" folder.
	void loadLocales();

	QVBoxLayout mLayout;
	QLabel mTitle;
	QListWidget mLanguages;
	QMap<QString, QString> mAvailableLocales;
};

}
