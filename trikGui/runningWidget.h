/* Copyright 2013 Yurii Litvinov
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
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
#endif

#include <QtCore/QList>
#include <QtCore/QString>

#include "controller.h"
#include <trikKernel/mainWidget.h>

namespace trikGui {

class RunningWidget : public trikKernel::MainWidget
{
	Q_OBJECT

public:
	explicit RunningWidget(QString const &programName, Controller &controller, QWidget *parent = 0);

	void showError(QString const &error);
	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	QVBoxLayout mLayout;
	QLabel mStatusLabel;
	QLabel mAbortLabel;
	Controller &mController;
};

}
