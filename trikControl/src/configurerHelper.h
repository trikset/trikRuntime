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

#include <QtGlobal>

class QString;

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class DeviceState;

/// Helper for trikKernel::Configurer.
class ConfigurerHelper
{
public:
	/// Reads integer parameter from configurer, modifies device state. Returns 0 if parameter is incorrect.
	/// @param configurer - configurer object from which parameter will be read.
	/// @param state - reference to device state, will be set to "fail" if parameter can not be read correctly.
	/// @param port - port of a device.
	/// @param parameterName - name of a parameter to read.
	static int configureInt(const trikKernel::Configurer &configurer, DeviceState &state, const QString &port
			, const QString &parameterName);

	/// Reads long integer parameter from configurer, modifies device state. Returns 0 if parameter is incorrect.
	/// @param configurer - configurer object from which parameter will be read.
	/// @param state - reference to device state, will be set to "fail" if parameter can not be read correctly.
	/// @param port - port of a device.
	/// @param parameterName - name of a parameter to read.
	static long configureLong(const trikKernel::Configurer &configurer, DeviceState &state, const QString &port
							, const QString &parameterName);

	/// Reads real parameter from configurer, modifies device state. Returns 0.0 if parameter is incorrect.
	/// @param configurer - configurer object from which parameter will be read.
	/// @param state - reference to device state, will be set to "fail" if parameter can not be read correctly.
	/// @param port - port of a device.
	/// @param parameterName - name of a parameter to read.
	static qreal configureReal(const trikKernel::Configurer &configurer, DeviceState &state, const QString &port
			, const QString &parameterName);
};

}
