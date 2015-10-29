/* Copyright 2015 CyberTech Labs Ltd.
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
#else
	#include <QtWidgets/QWidget>
#endif

namespace trikGui {

/// Base class for a widget that can show some reading from sensor, encoder and so on.
class AbstractIndicator : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param parent - parent of this widget in Qt widget parent-child system.
	explicit AbstractIndicator(QWidget *parent = 0) : QWidget(parent) {}

public slots:
	/// Rereads sensor and updates widget contents.
	virtual void renew() = 0;
};

}
