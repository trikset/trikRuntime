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

#include <QtCore/QTimer>
#include <QtCore/QVector>

#include "trikGuiDialog.h"

namespace trikControl {
	class Brick;
}

namespace trikGui {

class SensorIndicator;

/// Widget that shows current readings of selected sensors.
class SensorsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SensorsWidget(trikControl::Brick &brick, QStringList const &ports, QWidget *parent = 0);
	~SensorsWidget();
	int exec() override;
	void renewFocus() override;

protected:
	void exit();
	void goHome();

private:
	trikControl::Brick &mBrick;
	QVBoxLayout mLayout;
	QVector<SensorIndicator *> mIndicators;  // Has ownership.
	int const mInterval;
	QTimer mTimer;
};

}
