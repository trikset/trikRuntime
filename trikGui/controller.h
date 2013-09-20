/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
