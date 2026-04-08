/* Copyright 2013 - 2015 Yurii Litvinov, CyberTech Labs Ltd.
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

#include "runningCode.h"

using namespace trikGui;

RunningCode::RunningCode(Controller &controller, QObject *parent) : QObject(parent), mController(controller) {
	connect(&mController, &Controller::showRunningWidget, this, &RunningCode::showRunningCode);

	connect(&mController, &Controller::showError, this, &RunningCode::showError);
	connect(&mController, &Controller::hideRunningWidget, this, &RunningCode::hideRunningCode);
	connect(&mController, &Controller::hideScriptWidgets, this, &RunningCode::hideScript);
}

void RunningCode::showError(const QString &error, int scriptId) {
	if (mScriptId != scriptId) {
		return;
	}
	mError = error;
	Q_EMIT showErrorRunningCodeComponent(error);
}

void RunningCode::setProgram(const QString &programName, int scriptId) {
	mProgramName = programName;
	mScriptId = scriptId;
}
void RunningCode::abortScript()
{
	mController.abortExecution();
}

int RunningCode::scriptId() const { return mScriptId; }

void RunningCode::showRunningCode(const QString &fileName, int scriptId) {
	setProgram(fileName, scriptId);

	Q_EMIT showRunningCodeComponent(fileName);
}

void RunningCode::hideRunningCode(int scriptId) {
	if (mScriptId != scriptId) {
		return;
	}
	Q_EMIT hideRunningCodeComponent();
}

void RunningCode::hideScript() { Q_EMIT hideScriptComponent(); }