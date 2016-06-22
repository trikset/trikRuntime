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
	#include <QtGui/QLabel>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QListWidget>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListWidget>
#endif

#include <QtCore/QString>

#include <trikKernel/rcReader.h>
#include <trikWiFi/trikWiFi.h>
#include "trikGuiDialog.h"

namespace trikGui {

/// Widget which allows to set wi-fi mode (client or access point) and then opens corresponding configuration widget.
class WiFiModeWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	enum Mode {
		accessPoint
		, client
		, unknown
	};

	/// Constructor
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit WiFiModeWidget(trikWiFi::TrikWiFi &wiFi, QWidget *parent = 0);

	/// Returns menu entry for this widget.
	static QString menuEntry();

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	void setMode(Mode mode);

	trikWiFi::TrikWiFi &mWiFi;
	trikKernel::RcReader mRcReader;
	QVBoxLayout mLayout;
	QLabel mTitle;
	QListWidget mModes;
};

}
