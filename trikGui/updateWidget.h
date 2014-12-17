/* Copyright 2014 CyberTech Labs Ltd.
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
	#include <QtGui/QHBoxLayout>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
	#include <QtWidgets/QLabel>
#endif

#include "trikGuiDialog.h"

namespace trikGui {

/// Widget that allows to update a TrikRuntime.
class UpdateWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent of this widget in terms of Qt widget parent/child system.
	explicit UpdateWidget(QWidget *parent = 0);

	/// Destructor.
	~UpdateWidget();

	/// String that shall appear in menu for this widget.
	static QString menuEntry();

	int exec() override;

public slots:
	void renewFocus() override;

private:
	void showStatus(QString const &text, bool isError = false);

	/// Main layout of this widget.
	QHBoxLayout mLayout;

	QLabel mStatusLabel;
};

}
