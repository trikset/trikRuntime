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

#include <QtCore/QList>
#include <QtCore/QTimer>

#include "scriptInterface.h"

namespace trikControl {

/// Implementation of script controller for real robot.
class Script : public ScriptInterface
{
	Q_OBJECT

public:
	~Script() override;

	bool isInEventDrivenMode() const override;

	void reset() override;

public slots:
	QTimer *timer(int milliseconds) override;

	void wait(int const &milliseconds) override;

	qint64 time() const override;

	void run() override;

	void quit() override;

	void system(QString const &command) override;

private:
	QList<QTimer *> mTimers; // Has ownership.

	/// True, if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool mInEventDrivenMode = false;
};

}
