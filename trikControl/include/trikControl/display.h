/* Copyright 2013 Yurii Litvinov
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
	explicit Display(QThread &guiThread);

	~Display();

public slots:
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

	/// Shortcut to showImage, shows happy smile.
	void smile();

    /// Draw line on the widget.
    /// @param x1 - first point's x coordinate.
    /// @param y1 - first point's y coordinate.
    /// @param x1 - second point's x coordinate.
    /// @param y1 - second point's y coordinate.
    void drawLine(int x1, int y1, int x2, int y2);

    /// Delete all lines.
    void deleteAllLines();

    /// Draw point on the widget.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    void drawPoint(int x, int y);

    /// Delete all points.
    void deleteAllPoints();

    /// Draw rect on the widget.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    /// @param width - rect's width.
    /// @param height - rect's height.
    void drawRect(int x, int y, int width, int height);

    /// Delete all rects.
    void deleteAllRects();

    /// Draw ellipse.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    /// @param width - width of ellipse.
    /// @param height - height of ellipse.
    void drawEllipse(int x, int y, int width, int height);

    /// Delete all ellipses.
    void deleteAllEllipses();

    /// Draw arc on the widget.
    /// @param x - x coordinate.
    /// @param y - y coordinate.
    /// @param width - width rect forming an arc.
    /// @param height - height rect forming an arc.
    /// @param startAngle - start angle.
    /// @param spanAngle - end andle.
    void drawArc(int x, int y, int width, int height, int startAngle, int spanAngle);

    /// Delete all arcs.
    void deleteAllArcs();

	/// Shortcut to showImage, shows sad smile.
	void sadSmile();

	/// Sets background for a picture.
	/// @param color - color of a background.
	void setBackground(QString const &color);

	/// Hides and clears widget on which everything is drawn.
	void hide();

	/// Clear everything painted with this object.
	void clear();

signals:
	void threadShowImage(QString const &fileName);
	void threadAddLabel(QString const &text, int x, int y);
	void threadRemoveLabels();
	void threadSetBackground(QString const &color);
	void threadHide();
	void threadDelete();
	void threadClear();
    void threadDrawLine(int x1, int y1, int x2, int y2);
    void threadDeleteAllLines();
    void threadDrawPoint(int x, int y);
    void threadDeleteAllPoints();
    void threadDrawRect(int x, int y, int width, int height);
    void threadDeleteAllRects();
    void threadDrawEllipse(int x, int y, int width, int height);
    void threadDeleteAllEllipses();
    void threadDrawArc(int x, int y, int width, int height, int startAngle, int spanAngle);
    void threadDeleteAllArcs();

private:
	QThread &mGuiThread;
	GuiWorker *mGuiWorker;  // Has ownership.
};

}
