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

#include <QtCore/QThread>
#include <QtCore/QString>

#include "declSpec.h"

namespace trikKernel {
class Configurer;
}

namespace trikNetwork {

class GamepadInterface;

/// Factory that creates real gamepad receiver object.
class TRIKNETWORK_EXPORT GamepadFactory
{
public:
	/// Method that creates gamepad object.
	/// Transfers ownership over GamepadInterface object to caller.
	/// @param port - TCP port of a gamepad server.
	static GamepadInterface *create(int port);

	/// Method that creates gamepad object.
	/// Transfers ownership over GamepadInterface object to caller.
	/// @param configurer - configurer object that contains preparsed XML config. If gamepad is not enabled, nullptr
	///        will be returned.
	static GamepadInterface *create(trikKernel::Configurer const &configurer);
};

}
