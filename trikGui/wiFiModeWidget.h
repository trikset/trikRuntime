/* Copyright 2014 Roman Kurbatov
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
 * limitations under the License.
 */

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QLabel>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QListWidget>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListWidget>
#endif

#include <QtCore/QEventLoop>
#include <QtCore/QString>

#include "rcReader.h"

namespace trikGui {

/// Widget which allows to set network mode (client or access point) and then opens corresponding configuration widget.
class WiFiModeWidget : public QWidget
{
	Q_OBJECT

public:
	enum Mode {
		accessPoint
		, client
	};

	/// Constructor
	/// @param configPath - full path to configuration files.
	/// @param parent - parent of this widget in Qt object hierarchy.
	WiFiModeWidget(QString const &configPath, QWidget *parent = 0);

	/// Show the widget and wait until it will finish its work.
	void exec();

	/// Returns menu entry for this widget.
	static QString menuEntry();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	void setClient();
	void setAccessPoint();

	QString const &mConfigPath;
	RcReader rcReader;
	QEventLoop mEventLoop;
	QVBoxLayout mLayout;
	QLabel mTitle;
	QListWidget mModes;
};

}
