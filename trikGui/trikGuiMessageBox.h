/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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
	#include <QtGui/QWidget>
	#include <QtGui/QLabel>
	#include <QtGui/QVBoxLayout>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
#endif

#include <QtCore/QString>
#include <QtCore/QEventLoop>
#include <QtGui/QKeyEvent>

#include "mainWidget.h"

namespace trikGui {

/// Widget showing some message and waiting until the user will press any key.
class TrikGuiMessageBox : public MainWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit TrikGuiMessageBox(QWidget *parent = 0);

	/// Show widget and wait until the user will press any key.
	/// @param message - message to show.
	int exec(const QString &message);

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void powerDownHandler() override;

private:
	QVBoxLayout mLayout;
	QLabel mMessageLabel;
	QEventLoop mEventLoop;
};

}
