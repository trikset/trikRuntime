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
	#include <QtGui/QHBoxLayout>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
#endif

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
	#include <QtGui/QHBoxLayout>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
#endif

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QVector>

namespace trikControl {
	class Brick;
}

namespace trikGui {

class SensorIndicator;

class SensorsWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SensorsWidget(trikControl::Brick &brick, QStringList const &ports, QWidget *parent = 0);
	~SensorsWidget();
	void exec();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	trikControl::Brick &mBrick;
	QHBoxLayout mLayout;
	QVector<SensorIndicator *> mIndicators;
	QEventLoop mEventLoop;
	int const mInterval;
	QTimer mTimer;
};

}
