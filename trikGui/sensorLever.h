/* Copyright 2016 Mikhail Kita
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
	#include <QtGui/QLabel>
	#include <QtGui/QProgressBar>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QHBoxLayout>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QProgressBar>
#endif

#include <QtCore/QString>

namespace trikGui {

/// Widget that allows to set distance value of a sensor.
class SensorLever : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - name of a port which the sensor is connected to.
	/// @param parent - pointer to a parent widget.
	SensorLever(const QString &port, QWidget *parent = 0);

	/// Returns distance value of this lever.
	int distance() const;

	/// Returns sign of command.
	bool isGreater() const;

protected:
	void keyPressEvent(QKeyEvent *event) override;

	void paintEvent(QPaintEvent *) override;

private slots:
	/// Sets the distance which is necessary to trigger sensor.
	/// @param distance - required distance.
	void setDistance(int distance);

	/// Changes sign of this lever.
	void changeSign();

private:
	bool mIsGreater;
	const int mMaxDistance;
	const int mMinDistance;
	const int mDistanceStep;
	int mDistance;

	QHBoxLayout mLayout;
	QLabel mNameLabel;
	QProgressBar mDistanceBar;
	QLabel mSignLabel;
};

}
