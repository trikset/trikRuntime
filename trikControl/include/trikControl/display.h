/* Copyright 2013 - 2014 Yurii Litvinov, Smirnov Mikhail, Kogutich Denis
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

#include "declSpec.h"

namespace trikControl {

class GuiWorker;

/// Provides ability to draw something on robot display.
class TRIKCONTROL_EXPORT Display : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param guiThread - GUI thread of an application.
	/// @param startDirPath - path to the directory from which the application was executed.
	explicit Display(QThread &guiThread, QString const &startDirPath);

	~Display();

public slots:

	QString startDirPath() const;

	/// Shows given image on a display.
	/// @param fileName - file name (with path) of an image to show. Refer to Qt documentation for
	/// supported formats, but .jpg, .png, .bmp, .gif are supported.
	void showImage(QString const &fileName);

	/// Add a label to the specific position of the screen. If there already is a label in these coordinates, its
	/// contents will be updated.
	/// @param text - label text.
	/// @param x - label x coordinate.
	/// @param y - label y coordinate.
	void addLabel(QString const &text, int x, int y);

	/// Remove all labels from the screen.
	void removeLabels();

	/// Set painter color.
	void setPainterColor(QString const &color);

	/// Set painter width.
	void setPainterWidth(int penWidth);

	/// Draw line on the widget.
	/// @param x1 - first point's x coordinate.
	/// @param y1 - first point's y coordinate.
	/// @param x1 - second point's x coordinate.
	/// @param y1 - second point's y coordinate.
	void drawLine(int x1, int y1, int x2, int y2);

	/// Draw point on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	void drawPoint(int x, int y);

	/// Draw rect on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - rect's width.
	/// @param height - rect's height.
	void drawRect(int x, int y, int width, int height);

	/// Draw ellipse.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width of ellipse.
	/// @param height - height of ellipse.
	void drawEllipse(int x, int y, int width, int height);

	/// Draw arc on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	/// @param width - width rect forming an arc.
	/// @param height - height rect forming an arc.
	/// @param startAngle - start angle.
	/// @param spanAngle - end andle.
	void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle);

	/// Sets background for a picture.
	/// @param color - color of a background.
	void setBackground(QString const &color);

	/// Hides and clears widget on which everything is drawn.
	void hide();

	/// Clear everything painted with this object.
	void clear();

private:
	QThread &mGuiThread;
	QString const mStartDirPath;
	GuiWorker *mGuiWorker;  // Has ownership.
};

}
