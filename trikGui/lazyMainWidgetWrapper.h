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

#include <trikKernel/lazyMainWidget.h>
#include <trikControl/displayWidgetInterface.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QHBoxLayout>
#else
	#include <QtWidgets/QHBoxLayout>
#endif

namespace trikGui {

/// Wrapper that adapts DisplayWidgetInterface to LazyMainWidget.
class LazyMainWidgetWrapper : public trikKernel::LazyMainWidget
{
	Q_OBJECT

public:
	/// Constructor.
	LazyMainWidgetWrapper(trikControl::DisplayWidgetInterface *wrappedWidget, QWidget *parent = 0)
			: LazyMainWidget(parent), mWrappedWidget(wrappedWidget)
	{
		connect(wrappedWidget, SIGNAL(shown()), this, SLOT(emitShowMe()));
		connect(wrappedWidget, SIGNAL(hidden()), this, SIGNAL(hideMe()));

		const auto layout = new QHBoxLayout(this);

		layout->addWidget(wrappedWidget);
	}

	void renewFocus() override
	{
		mWrappedWidget->setFocus();
	}

signals:
	/// Emitted when the widget wants it to be shown.
	void showMe(trikKernel::MainWidget &widget);

	/// Emitted when the widget wants it to be hidden.
	void hideMe();

private slots:
	/// Helper slot to re-emit a signal with correct parameter (life without Qt5 is pain).
	void emitShowMe()
	{
		emit showMe(*this);
	}

private:
	QWidget * const mWrappedWidget;  // Does not have ownership.
};
}

