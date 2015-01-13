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
#include <QtCore/QHash>

class QDomElement;

namespace trikKernel {

/// Generic configuration helper, parses configuration XML files and presents configuration as a set of attributes
/// with their values for each configurable device. Respects configuration overriding rules:
/// Model config > device type config > device-port pair config > device class config
class Configurer
{
public:
	Configurer(QString const &pathToSystemConfig, QString const &pathToModelConfig);

	QString attribute(QString const &deviceName, QString const &attributeName) const;

	QString attribute(QString const &deviceName, QString const &port, QString const &attributeName) const;

	bool isConfigured(QString const deviceName) const;

private:
	struct Device {
		QString name;
		QHash<QString, QString> attributes;
		QHash<QString, QHash<QString, QString>> portSpecificAttributes;
	};

	struct DeviceType {
		QString name;
		QString deviceClass;
		QHash<QString, QString> attributes;
	};

	void parseDeviceClasses(QDomElement const &element);
	void parseDevicePorts(QDomElement const &element);
	void parseDeviceTypes(QDomElement const &element);
	void parseInitScript(QDomElement const &element);
	void parseAdditionalConfigurations(QDomElement const &element);
	void parseModelConfig(QDomElement const &element);

	QStringList mInitScripts;
	QHash<QString, Device> mDevices;
	QHash<QString, DeviceType> mDeviceTypes;
	QHash<QString, Device> mAdditionalConfiguration;
};

}
