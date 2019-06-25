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
#include <QVBoxLayout>

#include "trikGuiDialog.h"
#include "updateWidget.h"

namespace trikGui {

/// Widget that shows current version of TrikRuntime.
class InformationWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent of this widget in terms of Qt widget parent/child system.
	explicit InformationWidget(QWidget *parent = nullptr);

	/// Destructor.
	~InformationWidget() override = default;

	/// String that shall appear in menu for this widget.
	static QString menuEntry();

public slots:
	void renewFocus() override {}

private:
	/// Returns operating system version.
	QString osVersion() const;

	/// Main layout of this widget.
	QVBoxLayout mLayout;
};

}
