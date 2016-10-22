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
	#include <QtGui/QScrollArea>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QScrollArea>
#endif

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <trikControl/brickInterface.h>

#include "trikGuiDialog.h"

namespace trikGui {

class MotorLever;

/// Widget that allows to test motors connected to TRIK controller
class MotorsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param brick - reference to trikControl::Brick instance.
	/// @param type - type of motors we want to test.
	/// @param parent - pointer to a parent widget.
	MotorsWidget(trikControl::BrickInterface &brick
				, trikControl::MotorInterface::Type type
				, QWidget *parent = 0);

	/// Destructor.
	~MotorsWidget() override;

	/// Title for this widget in a main menu.
	static QString menuEntry(trikControl::MotorInterface::Type type);

	void renewFocus() override;

	bool eventFilter(QObject *, QEvent *event) override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	void ensureFocusedWidgetVisible();

private slots:
	/// Hack to work around odd QScrollArea behavior which incorrectly positions inner widget when there is no need for
	/// vertical scrollbar.
	void fixLeversPosition();

private:
	trikControl::BrickInterface &mBrick;
	QStringList mPorts;
	QVector<MotorLever *> mLevers; // Has ownership.
	QScrollArea *mScrollArea; // Has ownership.
};

}
