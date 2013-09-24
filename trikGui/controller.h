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

#include <trikCommunicator/trikCommunicator.h>
#include <trikScriptRunner/trikScriptRunner.h>

namespace trikGui
{

/// Provides backend features like Qt Script runner for frontend.
class Controller
{
public:
	Controller();

	/// Executes specified file as Qt Script, if it has .qts extension, or as a program otherwise.
	void runFile(QString const &filePath);

private:
	trikScriptRunner::TrikScriptRunner mScriptRunner;
	trikCommunicator::TrikCommunicator mCommunicator;
};

}
