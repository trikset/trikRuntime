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
#include <type_traits>

#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>

#include <QsLog.h>

#include "deviceState.h"

namespace trikControl {

/// Helper for trikKernel::Configurer.
class ConfigurerHelper
{
public:
	/// Reads integer parameter from configurer, modifies device state. Returns 0 if parameter is incorrect.
	inline static int configureInt(const trikKernel::Configurer &configurer, DeviceState &state
			, const QString &port, const QString &parameterName)
	{
		return configure<int>(state, parameterName
				, [&]() { return configurer.attributeByPort(port, parameterName); });
	}

	/// Reads long integer parameter from configurer, modifies device state. Returns 0 if parameter is incorrect.
	inline static long configureLong(const trikKernel::Configurer &configurer, DeviceState &state
			, const QString &port, const QString &parameterName)
	{
		return configure<long>(state, parameterName
				, [&]() { return configurer.attributeByPort(port, parameterName); });
	}

	/// Reads real parameter from configurer, modifies device state. Returns 0.0 if parameter is incorrect.
	inline static qreal configureReal(const trikKernel::Configurer &configurer, DeviceState &state
			, const QString &port, const QString &parameterName)
	{
		return configure<qreal>(state, parameterName
				, [&]() { return configurer.attributeByPort(port, parameterName); });
	}

	/// Reads integer child parameter by port, modifies device state. Returns 0 if parameter is incorrect.
	inline static int configureChildInt(const trikKernel::Configurer &configurer, DeviceState &state
			, const QString &port, const QString &childDeviceClass, const QString &parameterName)
	{
		return configure<int>(state, parameterName
				, [&]() { return configurer.childAttributeByPort(port, childDeviceClass, parameterName); });
	}

	/// Reads long integer child parameter by port, modifies device state. Returns 0 if parameter is incorrect.
	inline static long configureChildLong(const trikKernel::Configurer &configurer, DeviceState &state
			, const QString &port, const QString &childDeviceClass, const QString &parameterName)
	{
		return configure<long>(state, parameterName
				, [&]() { return configurer.childAttributeByPort(port, childDeviceClass, parameterName); });
	}

	/// Reads real child parameter by port, modifies device state. Returns 0.0 if parameter is incorrect.
	inline static qreal configureChildReal(const trikKernel::Configurer &configurer, DeviceState &state
			, const QString &port, const QString &childDeviceClass, const QString &parameterName)
	{
		return configure<qreal>(state, parameterName
				, [&]() { return configurer.childAttributeByPort(port, childDeviceClass, parameterName); });
	}

	/// Reads parameter via a callable, modifies device state.
	/// @param state - device state, will be set to "fail" on error.
	/// @param parameterName - name of a parameter (for error logging).
	/// @param query - callable returning QString, captures configurer internally if needed.
	/// @param args - arguments forwarded to the callable.
	template<typename T, typename Fn, typename... Args>
	static T configure(DeviceState &state, const QString &parameterName, Fn &&query, Args&&... args)
	{
		try {
			const QString raw = std::forward<Fn>(query)(std::forward<Args>(args)...);
			bool ok = false;
			T parameter = convert<T>(raw, ok);
			if (!ok) {
				QLOG_ERROR() << QString(R"(Incorrect configuration for parameter "%1": "%2")")
						.arg(parameterName, raw);
				state.fail();
				return {};
			}

			return parameter;
		} catch (trikKernel::MalformedConfigException &) {
			state.fail();
			return {};
		}
	}

private:
	template<typename T>
	static typename std::enable_if<std::is_same<T, int>::value, T>::type
	convert(const QString &s, bool &ok)
	{
		return s.toInt(&ok, 0);
	}

	template<typename T>
	static typename std::enable_if<std::is_same<T, long>::value, T>::type
	convert(const QString &s, bool &ok)
	{
		return s.toLong(&ok, 0);
	}

	template<typename T>
	static typename std::enable_if<std::is_same<T, qreal>::value, T>::type
	convert(const QString &s, bool &ok)
	{
		return s.toDouble(&ok);
	}
};

}
