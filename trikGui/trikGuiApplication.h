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
 * limitations under the License. */

#pragma once

#include <QGuiApplication>
#include <QTimer>
#include <QtQml/qqml.h>

namespace trikGui {

/// Custom application class that handles the power button press sequence:
/// short press → show confirmation dialog (via showPowerConfirm signal),
/// continued hold → shutdown.
class TrikGuiApplication : public QGuiApplication
{
	Q_OBJECT
public:
	TrikGuiApplication(int &argc, char **argv);

	bool eventFilter(QObject *obj, QEvent *event) override;

Q_SIGNALS:
	void showPowerConfirm();
	void hidePowerConfirm();

private Q_SLOTS:
	void shutdownSoon();
	void shutdown();

private:
	void forceRepaint();

	QTimer mPowerButtonPressedTimer;
	QTimer mShutdownDelayTimer;
	bool mIsShutdownRequested{false};
};

} // namespace trikGui
