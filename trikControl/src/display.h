/* Copyright 2013 - 2015 Yurii Litvinov, Smirnov Mikhail, Kogutich Denis
 * and CyberTech Labs Ltd.
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

#include <QtCore/QThread>
#include <QtCore/QString>

#include "displayInterface.h"

namespace trikControl {

class GuiWorker;

/// Implementation of display interface for real robot.
class Display : public DisplayInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param guiThread - GUI thread of an application.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	explicit Display(const QString &mediaPath);

	~Display() override;

	DisplayWidgetInterface &graphicsWidget() override;

public slots:
	void setBackground(const QString &color) override;
	void showImage(const QString &fileName) override;

	void addLabel(const QString &text, int x, int y) override;
	void removeLabels() override;

	void setPainterColor(const QString &color) override;
	void setPainterWidth(int penWidth) override;

	void drawLine(int x1, int y1, int x2, int y2) override;
	void drawPoint(int x, int y) override;
	void drawRect(int x, int y, int width, int height, bool filled = false) override;
	void drawEllipse(int x, int y, int width, int height, bool filled = false) override;
	void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle) override;

	void hide() override;
	void clear() override;
	void reset() override;
	void redraw() override;

private:
	const QString mMediaPath;
	GuiWorker *mGuiWorker;  // Has ownership.
};

}
