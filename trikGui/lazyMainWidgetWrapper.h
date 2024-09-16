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
	#include <QtGui/QHBoxLayout>
#else
	#include <QtWidgets/QHBoxLayout>
#endif

#include <trikControl/displayWidgetInterface.h>

#include "lazyMainWidget.h"

namespace trikGui {

/// Wrapper that adapts DisplayWidgetInterface to LazyMainWidget.
class LazyMainWidgetWrapper : public LazyMainWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param wrappedWidget - widget to be wrapped into LazyMainWidget interface. Does not take ownership.
	/// @param parent - parent of this wrapper in Qt Widgets parent/child system.
	LazyMainWidgetWrapper(trikControl::DisplayWidgetInterface *wrappedWidget, QWidget *parent = nullptr);

	~LazyMainWidgetWrapper() override;
	void renewFocus() override;

private slots:
	/// Helper slot to re-emit a signal with correct parameter (life without Qt5 is pain).
	void emitShowMe();

private:
	/// Wrapped display widget.
	/// Does not have ownership.
	QWidget * const mWrappedWidget;

	/// Layout for correct placement of wrapped widget.
	/// Has ownership by Qt parent-child system.
	QHBoxLayout * const mLayout;
};
}
