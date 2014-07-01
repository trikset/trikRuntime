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
 * limitations under the License. */

#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
#endif

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <trikControl/brick.h>

#include "trikGuiDialog.h"

namespace trikGui {

class MotorLever;

/// Widget that allows to test motors connected to TRIK controller
class MotorsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param configPath - path to config.xml (TrikControl configuration file).
	/// @param parent - pointer to a parent widget.
	/// @param startDirPath - path to the directory from which the application was executed.
	MotorsWidget(QString const &configPath
			, QString const &startDirPath
			, trikControl::Motor::Type type
			, QWidget *parent = 0
			);

	/// Destructor.
	~MotorsWidget();

	/// Title for this widget in a main menu.
	static QString menuEntry(trikControl::Motor::Type type);

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	QVBoxLayout mLayout;

	/// @todo Why does it need its own brick?
	trikControl::Brick mBrick;
	QStringList mPorts;
	QVector<MotorLever *> mLevers; // Has ownership.
};

}
