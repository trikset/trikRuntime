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
	#include <QtGui/QLabel>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QListWidget>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QListWidget>
#endif

#include <QtCore/QString>

#include <trikControl/brickInterface.h>

#include "trikGuiDialog.h"

namespace trikGui {

class SensorLever;
class ConnectButton;

/// Widget which allows to change settings of the robot sensors.
class SensorSettingsWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor
	/// @param port - name of port for this sensor
	/// @param isEncoder - flag that shows whether this sensor is encoder
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit SensorSettingsWidget(const QString &port, bool isEncoder, QWidget *parent = 0);

	~SensorSettingsWidget();

	/// Returns menu entry for this widget.
	static QString menuEntry();

	void renewFocus() override;

	/// Generates new command for script and returns it.
	QString createScript();

protected:
	void keyPressEvent(QKeyEvent *event) override;

private slots:
	/// Called by child widgets when they need to move input focus up or down.
	void focus();

private:
	QEventLoop mEventLoop;
	QVBoxLayout mLayout;
	QString mPort;
	bool mIsEncoder;
	QLabel mPowerLabel;

	QScopedPointer<SensorLever> mLever;

	/// Button that returns to previous screen.
	QScopedPointer<ConnectButton> mContinueButton;
};

}
