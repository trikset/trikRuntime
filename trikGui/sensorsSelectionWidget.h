/* Copyright 2014 - 2015 Roman Kurbatov and CyberTech Labs Ltd.
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

#include <QtCore/QStringList>
#include <QtGui/QKeyEvent>

#include <trikControl/brickInterface.h>
#include <trikControl/sensorInterface.h>

#include "trikGuiDialog.h"

namespace trikGui {

/// Widget that allows to select sensors for testing.
class SensorsSelectionWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	enum class SensorType {
		analogSensor
		, digitalSensor
		, encoder
	};

	SensorsSelectionWidget(trikControl::BrickInterface &brick
			, SensorType type
			, QWidget *parent = 0);

	/// Returns main menu entry string for this widget.
	static QString menuEntry(SensorType type);

	void renewFocus() override;

protected:
	void keyPressEvent(QKeyEvent *event) override;

private:
	void activateItem();
	void startTesting();

	QVBoxLayout mLayout;
	QLabel mTitle;
	QListWidget mList;
	const SensorType mSensorType;

	trikControl::BrickInterface &mBrick;
};

}
