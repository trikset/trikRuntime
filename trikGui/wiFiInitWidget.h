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

#include <QtCore/QProcess>
#include <QtCore/QEventLoop>

#include "wiFiModeWidget.h"
#include "mainWidget.h"

namespace trikGui {

/// Widget for initializing wi-fi on the controller.
class WiFiInitWidget : public MainWidget
{
	Q_OBJECT

public:
	/// Constructor
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit WiFiInitWidget(QWidget *parent = 0);

	enum Result {
		success
		, fail
	};

	/// Initialize wi-fi on the controller.
	/// @param mode - wi-fi mode which we want to initialize.
	/// @return WiFiInitWidget::success if wi-fi was succesfully initialized and WiFiInitWidget::fail otherwise.
	Result init(WiFiModeWidget::Mode mode);

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	QEventLoop mEventLoop;
	QVBoxLayout mLayout;
	QLabel mInitMessage;
	QLabel mWaitMessage;
	QLabel mBreakMessage;
	QProcess mProcess;

private slots:
	void onProcessFinished(int, QProcess::ExitStatus exitStatus);
	void onProcessError(QProcess::ProcessError error);
};

}
