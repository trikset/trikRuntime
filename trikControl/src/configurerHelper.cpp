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

#include "configurerHelper.h"

#include <QtCore/QString>

#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>

#include <QsLog.h>

#include "deviceState.h"

using namespace trikControl;

int ConfigurerHelper::configureInt(const trikKernel::Configurer &configurer, DeviceState &state, const QString &port
		, const QString &parameterName)
{
	try {
		bool ok = false;
		int parameter = configurer.attributeByPort(port, parameterName).toInt(&ok, 0);
		if (!ok) {
			QLOG_ERROR() << QString(R"(Incorrect configuration for parameter "%1" for port "%2": "%3" )")
					.arg(parameterName).arg(port).arg(configurer.attributeByPort(port, parameterName));
			state.fail();
			return 0;
		}

		return parameter;
	} catch (trikKernel::MalformedConfigException &) {
		state.fail();
		return 0;
	}
}

long ConfigurerHelper::configureLong(const trikKernel::Configurer &configurer, DeviceState &state, const QString &port
								   , const QString &parameterName)
{
	try {
		bool ok = false;
		long parameter = configurer.attributeByPort(port, parameterName).toLong(&ok, 0);
		if (!ok) {
			QLOG_ERROR() << QString(R"(Incorrect configuration for parameter "%1" for port "%2": "%3" )")
								.arg(parameterName).arg(port).arg(configurer.attributeByPort(port, parameterName));
			state.fail();
			return 0;
		}

		return parameter;
	} catch (trikKernel::MalformedConfigException &) {
		state.fail();
		return 0;
	}
}

qreal ConfigurerHelper::configureReal(const trikKernel::Configurer &configurer, DeviceState &state, const QString &port
		, const QString &parameterName)
{
	try {
		bool ok = false;
		const qreal parameter = configurer.attributeByPort(port, parameterName).toDouble(&ok);
		if (!ok) {
			QLOG_ERROR() << QString(R"(Incorrect configuration for parameter "%1" for port "%2": "%3" )")
					.arg(parameterName).arg(port).arg(configurer.attributeByPort(port, parameterName));

			state.fail();
			return 0;
		}

		return parameter;
	} catch (trikKernel::MalformedConfigException &) {
		state.fail();
		return 0;
	}
}
