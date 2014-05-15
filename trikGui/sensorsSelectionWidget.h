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
	#include <QtGui/QListWidget>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListWidget>
	#include <QtWidgets/QLabel>
#endif

#include <QtCore/QEventLoop>
#include <QtCore/QStringList>
#include <QtGui/QKeyEvent>

#include <trikControl/brick.h>
#include <trikControl/sensor.h>

namespace trikGui {

class SensorsSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	SensorsSelectionWidget(QString const &configPath
			, trikControl::Sensor::Type type
			, QWidget *parent = 0);

	int exec();

	static QString menuEntry(trikControl::Sensor::Type type);

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	void activateItem();
	void startTesting();
	void exit();
	void goHome();

	QVBoxLayout mLayout;
	QLabel mTitle;
	QListWidget mList;
	trikControl::Brick mBrick;
	QEventLoop mEventLoop;
};

}
