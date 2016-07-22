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
#include <QtCore/QStringList>
#include <QtCore/QHash>

class QDomElement;

namespace trikKernel {

/// Generic configuration helper, parses configuration XML files and presents configuration as a set of attributes
/// with their values for each configurable device. Respects configuration overriding rules:
/// Model config > device type config > device-port pair config > device class config
class Configurer
{
public:
	/// Constructor.
	/// @param systemConfig - file name (with path) of system config, absolute or relative to current directory.
	/// @param modelConfig - file name (with path) of model config, absolute or relative to current directory.
	Configurer(const QString &systemConfig, const QString &modelConfig);

	/// Returns value of given attribute of given device.
	QString attributeByDevice(const QString &deviceClass, const QString &attributeName) const;

	/// Returns value of given attribute of a device on given port.
	QString attributeByPort(const QString &port, const QString &attributeName) const;

	/// Returns true if device is enabled in current configuration (either explicitly enabled in model configuration
	/// or can not be disabled at all).
	bool isEnabled(const QString deviceName) const;

	/// Ports configured in model config.
	QStringList ports() const;

	/// Returns class of a device configured on given port.
	QString deviceClass(const QString &port) const;

	/// Returns init scripts defined in config files, first from system config then from model config.
	QStringList initScripts() const;

	/// Changes configuration by configuring given device on given port. Port must be listed in model-config.xml,
	/// device shall be listed in system-config.xml, and device shall be able to be configured on a port
	/// (it is also described in system-config.xml).
	void configure(const QString &portName, const QString &deviceName);

	/// Returns version of the config file which shall correspond to casing model.
	QString version() const;

private:
	struct Device {
		QString name;
		QHash<QString, QString> attributes;
		QHash<QString, QHash<QString, QString>> portSpecificAttributes;
		bool isOptional = false;
	};

	struct DeviceType {
		QString name;
		QString deviceClass;
		QHash<QString, QString> attributes;
	};

	struct ModelConfigurationElement {
		QString port;
		QString deviceType;
		QHash<QString, QString> attributes;
	};

	struct AdditionalModelConfigurationElement {
		QString deviceClass;
		QHash<QString, QString> attributes;
	};

	void parseDeviceClasses(const QDomElement &element);
	void parseDevicePorts(const QDomElement &element);
	void parseDeviceTypes(const QDomElement &element);
	void parseInitScript(const QDomElement &element);
	void parseAdditionalConfigurations(const QDomElement &element);
	void parseModelConfig(const QDomElement &element);

	QStringList mInitScripts;

	/// Maps device class name to its configuration.
	QHash<QString, Device> mDevices;

	/// Maps device type name to its configuration.
	QHash<QString, DeviceType> mDeviceTypes;

	/// Maps device class name to its configuration.
	QHash<QString, Device> mAdditionalConfiguration;

	/// Maps port name to configuration of device on that port.
	QHash<QString, ModelConfigurationElement> mModelConfiguration;

	/// Maps device type name to its configuration.
	QHash<QString, AdditionalModelConfigurationElement> mAdditionalModelConfiguration;

	/// Version of the config file which shall correspond to casing model.
	QString mVersion;
};

}
