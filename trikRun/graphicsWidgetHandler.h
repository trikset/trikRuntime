/* Copyright 2014 CyberTech Labs Ltd.
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

#include <trikKernel/mainWidget.h>

namespace trikRun {

/// Helper class to show or hide graphics widget from brick.
class GraphicsWidgetHandler : public QWidget
{
	Q_OBJECT

public slots:
	/// Shows widget.
	void show(trikKernel::MainWidget &widget)
	{
		widget.show();
	}

	/// Hides widget.
	void hide()
	{
		auto widget = dynamic_cast<trikKernel::MainWidget *>(sender());
		if (widget) {
			widget->hide();
		}
	}
};

}
