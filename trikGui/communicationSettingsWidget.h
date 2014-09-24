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
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
	#include <QtGui/QPushButton>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QPushButton>
#endif

#include <trikControl/brick.h>

#include "trikGuiDialog.h"
#include "numberSelectionWidget.h"
#include "connectButton.h"

namespace trikGui {

class CommunicationSettingsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	CommunicationSettingsWidget(trikControl::Brick &brick, QWidget *parent = 0);

	static QString menuEntry();

public slots:
	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event);

private slots:
	void onConnectButtonClicked();
	void onHullNumberChanged(int newHullNumber);
	void focusUp();
	void focusDown();

private:
	QVBoxLayout mLayout;
	QLabel mTitle;
	QLabel mHelpLabel;
	QLabel mHullNumberLabel;
	NumberSelectionWidget mHullNumberSelector;
	QLabel mServerIpLabel;
	NumberSelectionWidget mServerIpSelector;
	ConnectButton mConnectButton;

	trikControl::Brick &mBrick;
};

}
