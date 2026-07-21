/* Copyright 2024 CyberTech Labs Ltd.
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

#include <atomic>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>
#include <trikScriptRunner/trikScriptControlInterface.h>
#include <trikControl/utilities.h>

namespace trikScriptRunner {

/// Abstract base class for user C++ programs running on TRIK robots.
///
/// Inherit from this class, implement startProgram(), and register your class
/// with the TRIK_REGISTER_CPP_PROGRAM macro. The runtime will instantiate your
/// class via the factory function, call startProgram() in a dedicated thread,
/// and set isTerminate to true when the program must stop.
///
/// Example:
/// @code
/// class MyProgram : public trikScriptRunner::TrikCppProgram {
/// public:
///     using TrikCppProgram::TrikCppProgram;
///     void startProgram(const std::atomic<bool> &isTerminate) override {
///         while (!isTerminate) {
///             mBrick->led()->red();
///             mScriptControl->wait(500);
///             mBrick->led()->green();
///             mScriptControl->wait(500);
///         }
///     }
/// };
/// TRIK_REGISTER_CPP_PROGRAM(MyProgram)
/// @endcode
class TrikCppProgram
{
public:
	TrikCppProgram(trikControl::BrickInterface *brick
			, trikNetwork::MailboxInterface *mailbox
			, TrikScriptControlInterface *scriptControl
			)
		: mBrick(brick)
		, mMailbox(mailbox)
		, mScriptControl(scriptControl)
	{}

	virtual ~TrikCppProgram() = default;

	/// Called by the runtime in a dedicated thread. Implement your robot logic here.
	/// @param isTerminate - becomes true when the runtime requests the program to stop.
	///        Check it in your main loop: while (!isTerminate) { ... }
	virtual void startProgram(const std::atomic<bool> &isTerminate) = 0;

	TrikCppProgram (const TrikCppProgram&) = delete;
	TrikCppProgram &operator=(const TrikCppProgram&) = delete;

	QVector<int32_t> getPhoto() {
		return trikControl::Utilities::rescalePhoto(mBrick->getStillImage());
	}

protected:
	trikControl::BrickInterface *mBrick;
	trikNetwork::MailboxInterface *mMailbox;
	TrikScriptControlInterface *mScriptControl;
};

/// Signature of the factory function that every user .so must export as extern "C".
using TrikCppProgramFactory = TrikCppProgram* (*)(
		trikControl::BrickInterface *brick,
		trikNetwork::MailboxInterface *mailbox,
		TrikScriptControlInterface *scriptControl
);

} // namespace trikScriptRunner

/// Name of the factory symbol resolved via QLibrary::resolve().
#define TRIK_CPP_PROGRAM_FACTORY_SYMBOL "createTrikCppProgram"

/// Registers @p ClassName as the entry point of the shared library.
/// Place this macro once in your .cpp file, after the class definition.
/// It generates an extern "C" factory function that the runtime uses to
/// instantiate your program class.

// NOTE: parentheses around ClassName would turn this into placement new — false positive.
// NOLINTBEGIN(bugprone-macro-parentheses)
#define TRIK_REGISTER_CPP_PROGRAM(ClassName) \
	extern "C" trikScriptRunner::TrikCppProgram *createTrikCppProgram( \
			trikControl::BrickInterface *brick, \
			trikNetwork::MailboxInterface *mailbox, \
			trikScriptRunner::TrikScriptControlInterface *scriptControl) \
	{ \
		return new ClassName(brick, mailbox, scriptControl); \
	}
// NOLINTEND(bugprone-macro-parentheses)
