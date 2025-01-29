/* Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#pragma once

#include <QtCore/qglobal.h>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>

namespace trikGui {

/// Application main class, to translate events from robot buttons into more convenient events
/// (like presses to Enter and Escape).
class TrikGuiApplication : public QApplication
{
	Q_OBJECT

public:
	/// Constructor with command-line arguments.
	TrikGuiApplication(int &argc, char **argv);

	bool notify(QObject *receiver, QEvent *event) override;

private Q_SLOTS:
	/// Temporary measure for some demo models which do not use QWS to draw their graphics, so we need to update
	/// all GUI to remove clutter on a screen.
	void refreshWidgets();

	void shutdownSoon();
	void shutdown();

private:
	QTimer mPowerButtonPressedTimer;
	QTimer mShutdownDelayTimer;
	QString mSavedStyleSheet;
	bool mIsShuttingDown = false;
	bool mIsShutdownRequested = false;
};

}
