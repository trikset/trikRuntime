/* Copyright 2014 CyberTech Labs Ltd.
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

#include <functional>
#include <type_traits>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

#include <trikControl/batteryInterface.h>
#include <trikControl/colorSensorInterface.h>
#include <trikControl/displayInterface.h>
#include <trikControl/encoderInterface.h>
#include <trikControl/eventCodeInterface.h>
#include <trikControl/eventDeviceInterface.h>
#include <trikControl/eventInterface.h>
#include <trikControl/gamepadInterface.h>
#include <trikControl/gyroSensorInterface.h>
#include <trikControl/i2cDeviceInterface.h>
#include <trikControl/irCameraInterface.h>
#include <trikControl/lineSensorInterface.h>
#include <trikControl/motorInterface.h>
#include <trikControl/objectSensorInterface.h>
#include <trikControl/soundSensorInterface.h>
#include <trikControl/sensorInterface.h>
#include <trikControl/vectorSensorInterface.h>
#include <trikControl/fifoInterface.h>
#include <trikControl/keysInterface.h>
#include <trikControl/ledInterface.h>
#include <trikControl/markerInterface.h>
#include <trikControl/lidarInterface.h>
#include <trikNetwork/mailboxInterface.h>

#include "trikScriptControlInterface.h"


#define REGISTER_METATYPE(TYPE) \
	qRegisterMetaType<TYPE*>(TYPE::staticMetaObject.className());

/// Here we define a convenient template that registers all devices used in trik.
/// When creating a new device(interface), you should append it to this list.
/// So it lets you write the device just one time rather than append appropriate line to each place
/// that uses devices.
/// ATTENTION: do not forget to append newly created device to this list!
#define REGISTER_DEVICES_WITH_TEMPLATE(TEMPLATE) \
	TEMPLATE(trikControl::BatteryInterface) \
	TEMPLATE(trikControl::ColorSensorInterface) \
	TEMPLATE(trikControl::FifoInterface) \
	TEMPLATE(trikControl::DisplayInterface) \
	TEMPLATE(trikControl::EncoderInterface) \
	TEMPLATE(trikControl::EventCodeInterface) \
	TEMPLATE(trikControl::EventDeviceInterface) \
	TEMPLATE(trikControl::EventInterface) \
	TEMPLATE(trikControl::GamepadInterface) \
	TEMPLATE(trikControl::GyroSensorInterface) \
	TEMPLATE(trikControl::I2cDeviceInterface) \
	TEMPLATE(trikControl::IrCameraInterface) \
	TEMPLATE(trikControl::KeysInterface) \
	TEMPLATE(trikControl::LedInterface) \
	TEMPLATE(trikControl::LineSensorInterface) \
	TEMPLATE(trikControl::MarkerInterface) \
	TEMPLATE(trikControl::MotorInterface) \
	TEMPLATE(trikControl::ObjectSensorInterface) \
	TEMPLATE(trikControl::SoundSensorInterface) \
	TEMPLATE(trikControl::SensorInterface) \
	TEMPLATE(trikControl::VectorSensorInterface) \
	TEMPLATE(trikNetwork::MailboxInterface) \
	TEMPLATE(trikControl::LidarInterface) \
	TEMPLATE(trikControl::PwmCaptureInterface) \

namespace trikScriptRunner {

enum class ScriptType { // must be 0, 1, ..
	UNINITIALIZED = -1,
	JAVASCRIPT,
#ifndef TRIK_NOPYTHON
	PYTHON,
#endif
	Size // should always be the last
};

static constexpr typename std::underlying_type<ScriptType>::type to_underlying(ScriptType t) noexcept
{
	return static_cast<std::underlying_type<ScriptType>::type>(t);
}

/// Interface for all script executors.
class TRIKSCRIPTRUNNER_EXPORT TrikScriptRunnerInterface : public QObject
{
	Q_OBJECT

public:
	/// Registers given C++ function as callable from script, with given name.
	virtual void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) = 0;

	/// Adds custom initialization steps when creating script engine (useful when used from outside of the TrikRuntime).
	virtual void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step) = 0;

	/// Gets all method names from executive objects (brick, script, etc.) from ScriptEngineWorker
	/// (useful when used from outside of the TrikRuntime).
	virtual QStringList knownMethodNames() const = 0;

	struct Helper
	{
		/// Helper function collects all methods names from given metaObject.
		/// If the returnType of given method name is registered in metaobject system,
		/// newMetaObject is constructed and procedure is called recursively for it.
		static void collectMethodNames(QSet<QString> &result, const QMetaObject *obj);
	};


	/// Get status of last direct command/script
	virtual bool wasError() = 0;

public slots:
	/// Executes given script asynchronously. If some script is already executing, it will be aborted.
	/// For event-driven mode (where script has brick.run() command) script counts as finished
	/// when it requests to quit by itself or was aborted. When script is finished, completed() signal will be emitted.
	/// @param script - script in Qt Script language to be executed.
	/// @param fileName - name of a file from which the script was loaded.
	/// @warning: The multithreaded script must not contain useful actions in the global context
	/// (function calls, variable initializations and so on in the global context is restricted).
	/// The reason is in non-thread-safety of script engine. We must run scripts by separate script engines
	/// and thus to have an opportunity to start concrete function from the given file. But QScriptEngine API
	/// has no such possibility so we should append function call to the end of the script. So if script will
	/// run some actions in the global context they will be invoked on each thread start.
	virtual void run(const QString &script, const QString &fileName = "") = 0;


	/// Executes given script as direct command, so it will use existing script execution environment (or create one
	/// if needed) and will not reset execution state before or after execution. Sequence of direct commands counts
	/// as finished when one of them directly requests to quit (by brick.quit() command), then robot will be stopped,
	/// execution state will reset and completed() signal will be emitted.
	/// @param command - script in Qt Script to be executed as direct command.
	virtual void runDirectCommand(const QString &command) = 0;

	/// Aborts script execution. completed() signal will be emitted when script will be actually aborted, robot will
	/// be stopped and execution state will be reset. Note that direct commands and scripts in event-driven mode will
	/// be stopped as well.
	virtual void abort() = 0;

	/// Plays "beep" sound.
	virtual void brickBeep() = 0;

	/// Set working directory -- path to all project files
	virtual void setWorkingDirectory(const QString &workingDir) = 0;

signals:
	/// Emitted when current script completes execution (for event-driven mode it means that script requested to quit
	/// or was aborted).
	/// @param error - localized error message if any error occured during script execution or empty string
	/// if everything is fine.
	/// @param scriptId - unique identifier of a script completed
	void completed(const QString &error, int scriptId);

	/// Emitted when new script from file started.
	/// @param fileName - name of a file from where the script was loaded.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedScript(const QString &fileName, int scriptId);

	/// Emitted when new direct script started.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedDirectScript(int scriptId);

	/// Emitted when a message must be sent to a desktop.
	void textInStdOut(const QString &text);
};

}
