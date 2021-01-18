/* Copyright 2021 CyberTech Labs Ltd.
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

#include <QtCore/QTimer>

#include "trikAbstractTimer.h"
#include "trikScriptRunnerDeclSpec.h"

namespace trikScriptRunner {

/// Timer implementation for real-life time
class TRIKSCRIPTRUNNER_EXPORT TrikRealTimer : public TrikAbstractTimer
{
public:
	TrikRealTimer();

	bool isTicking() const override;
	int interval() const override;
	void start() override;
	void start(int ms) override;
	void stop() override;
	void setInterval(int ms) override;
	void setRepeatable(bool repeatable) override;

private:
	QTimer mTimer;
};

}
