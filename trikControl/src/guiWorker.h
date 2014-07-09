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

#include <QtCore/qglobal.h>
#include <QtCore/QMultiHash>
#include <QtCore/QList>
#include <QtGui/QPixmap>
#include <QtGui/QFontMetrics>

#include "graphicsWidget.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
#endif

namespace trikControl {

/// Works in GUI thread and is responsible for all output to display.
class GuiWorker : public QObject
{
	Q_OBJECT

public:
	GuiWorker();

public slots:
	/// Shows image with given filename on display. Image is scaled to fill the screen and is cached on first read
	/// for better performance.
	void showImage(QString const &fileName);

	/// Add a label to the specific position of the screen. If there already is a label in these coordinates, its
	/// contents will be updated.
	/// @param text - label text.
	/// @param x - label x coordinate.
	/// @param y - label y coordinate.
	void addLabel(QString const &text, int x, int y);

	/// Remove all labels from the screen.
	void removeLabels();

	/// Queues worker object for deletion. It is actually deleted when control flow returns to event loop.
	void deleteWorker();

	/// Hides image widget.
	void hide();

	/// Sets background for a picture.
	/// @param color - color of a background.
	void setBackground(QString const &color);

	/// Set painter color.
	void setPainterColor(QString const &color);

	/// Clear everything painted with this object.
	void clear();

	/// Draw point on the widget.
	/// @param x - x coordinate.
	/// @param y - y coordinate.
	void drawPoint(int x, int y);

	/// Draw line on the widget.
	/// @param x1 - first point's x coordinate.
	/// @param y1 - first point's y coordinate.
	/// @param x2 - second point's x coordinate.
	/// @param y2 - second point's y coordinate.
	void drawLine(int x1, int y1, int x2, int y2);

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

private:
	void resetBackground();

	/// Returns existing label with given coordinates or NULL if no such label exists.
	QLabel *findLabel(int x, int y) const;

	GraphicsWidget mImageWidget;
	QLabel mImageLabel;
	QHash<QString, QPixmap> mImagesCache;
	QMultiHash<int, QLabel *> mLabels; // Has ownership.
	QFontMetrics mFontMetrics;
};

}
