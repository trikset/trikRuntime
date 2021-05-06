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

#include <QtCore/QString>

#include <trikControl/trikControlDeclSpec.h>

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class BrickInterface;

/// Factory that creates brick object for a real robot.
class TRIKCONTROL_EXPORT BrickFactory
{
public:
	/// Method that creates brick for real robot, it meant to be the only way to create brick outside TrikControl.
	/// Transfers ownership over BrickInterface object to caller.
	/// @param systemConfig - file name (with path) of system config, absolute or relative to current directory.
	/// @param modelConfig - file name (with path) of model config, absolute or relative to current directory.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	static BrickInterface *create(const QString &systemConfig, const QString &modelConfig, const QString &mediaPath);

	/// Convenience method that creates brick with default config files, "system-config.xml" and "model-config.xml".
	/// Transfers ownership over BrickInterface object to caller.
	/// @param configFilesPath - path to system-config.xml and model-config.xml.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	static BrickInterface *create(const QString &configFilesPath = ".", const QString &mediaPath = ".");

	/// Method that creates brick using given hardware abstraction layer, to be called from tests (they can use dummy
	/// hardware implementation to check trikControl configuration, device management and conversions capability).
	/// Transfers ownership over BrickInterface object to caller.
	/// @param hardwareAbstraction - hardware abstraction layer implementation.
	/// @param systemConfig - file name (with path) of system config, absolute or relative to current directory.
	/// @param modelConfig - file name (with path) of model config, absolute or relative to current directory.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	static BrickInterface *create(
			trikHal::HardwareAbstractionInterface &hardwareAbstraction
			, const QString &systemConfig
			, const QString &modelConfig
			, const QString &mediaPath);
};

}
