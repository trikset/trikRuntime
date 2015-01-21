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

#include "configurer.h"

#include <functional>

#include <QtCore/qglobal.h>
#include <QtXml/QDomElement>

#include "exceptions/malformedConfigException.h"
#include "fileUtils.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#define QDomNamedNodeMapLengthType unsigned
#else
	#define QDomNamedNodeMapLengthType int
#endif


using namespace trikKernel;

Configurer::Configurer(QString const &systemConfigFileName, QString const &modelConfigFileName)
{
	QDomElement const systemConfig = trikKernel::FileUtils::readXmlFile(systemConfigFileName);
	QDomElement const modelConfig = trikKernel::FileUtils::readXmlFile(modelConfigFileName);

	auto parseSection = [&systemConfig](QString const &sectionName, std::function<void(QDomElement const &)> action) {
		QDomNodeList const section = systemConfig.elementsByTagName(sectionName);
		if (section.size() != 1) {
			throw MalformedConfigException("'" + sectionName + "' element shall appear exactly once in config");
		}

		action(section.at(0).toElement());
	};

	parseSection("deviceClasses", [this](QDomElement const &element) { parseDeviceClasses(element); });
	parseSection("devicePorts", [this](QDomElement const &element) { parseDevicePorts(element); });
	parseSection("deviceTypes", [this](QDomElement const &element) { parseDeviceTypes(element); });

	parseSection("initScript", [this](QDomElement const &element) { parseInitScript(element); });

	parseAdditionalConfigurations(systemConfig);

	parseModelConfig(modelConfig);
}

QString Configurer::attributeByDevice(QString const &deviceType, QString const &attributeName) const
{
	if (mAdditionalModelConfiguration.contains(deviceType)
			&& mAdditionalModelConfiguration[deviceType].attributes.contains(attributeName))
	{
		return mAdditionalModelConfiguration[deviceType].attributes[attributeName];
	}

	if (mAdditionalConfiguration.contains(deviceType)
			&& mAdditionalConfiguration[deviceType].attributes.contains(attributeName))
	{
		return mAdditionalConfiguration[deviceType].attributes[attributeName];
	}

	if (mDeviceTypes.contains(deviceType)) {
		if (mDeviceTypes[deviceType].attributes.contains(attributeName)) {
			return mDeviceTypes[deviceType].attributes[attributeName];
		}

		QString const deviceClass = mDeviceTypes[deviceType].deviceClass;
		if (mDevices.contains(deviceClass) && mDevices[deviceClass].attributes.contains(attributeName)) {
			return mDevices[deviceClass].attributes[attributeName];
		}
	}

	if (mDevices.contains(deviceType) && mDevices[deviceType].attributes.contains(attributeName)) {
		return mDevices[deviceType].attributes[attributeName];
	}

	throw MalformedConfigException(QString("Unknown attribute '%1' of device '%2'").arg(attributeName).arg(deviceType));
}

QString Configurer::attributeByPort(QString const &port, QString const &attributeName) const
{
	if (!mModelConfiguration.contains(port)) {
		throw MalformedConfigException(QString("Port '%1' is not configured").arg(port));
	}

	if (mModelConfiguration[port].attributes.contains(attributeName)) {
		return mModelConfiguration[port].attributes[attributeName];
	}

	QString const &deviceType = mModelConfiguration.value(port).deviceType;

	if (mDeviceTypes.contains(deviceType)) {
		if (mDeviceTypes[deviceType].attributes.contains(attributeName)) {
			return mDeviceTypes[deviceType].attributes[attributeName];
		}

		QString const deviceClass = mDeviceTypes[deviceType].deviceClass;
		if (mDevices.contains(deviceClass)) {
			Device const &device = mDevices[deviceClass];
			if (device.portSpecificAttributes.contains(port)) {
				if (device.portSpecificAttributes[port].contains(attributeName)) {
					return device.portSpecificAttributes[port][attributeName];
				}
			}

			if (device.attributes.contains(attributeName)) {
				return device.attributes[attributeName];
			}

			if (!device.portSpecificAttributes.contains(port)) {
				throw MalformedConfigException(QString("Device type '%1' is not allowed on port %2.")
						.arg(deviceType).arg(port));
			}
		} else {
			throw MalformedConfigException(
					QString("Device type '%1' has device class '%2' which is not listed in 'deviceClasses' section.")
							.arg(deviceType).arg(deviceClass));
		}
	}

	if (mDevices.contains(deviceType)) {
		Device const &device = mDevices[deviceType];
		if (device.portSpecificAttributes.contains(port)) {
			if (device.portSpecificAttributes[port].contains(attributeName)) {
				return device.portSpecificAttributes[port][attributeName];
			}
		}

		if (device.attributes.contains(attributeName)) {
			return device.attributes[attributeName];
		}
	}

	throw MalformedConfigException(QString("Unknown attribute '%1' of device '%2' on port '%3'")
			.arg(attributeName).arg(deviceType).arg(port));
}

bool Configurer::isEnabled(QString const deviceName) const
{
	if (mAdditionalModelConfiguration.contains(deviceName)) {
		return true;
	}

	if (mDevices.contains(deviceName) && !mDevices.value(deviceName).isOptional) {
		return true;
	}

	return false;
}

QStringList Configurer::ports() const
{
	return mModelConfiguration.keys();
}

QString Configurer::deviceClass(QString const &port) const
{
	if (!mModelConfiguration.contains(port)) {
		throw MalformedConfigException(QString("Port '%1' is not configured").arg(port));
	}

	QString const &deviceType = mModelConfiguration.value(port).deviceType;
	if (mDeviceTypes.contains(deviceType)) {
		return mDeviceTypes.value(deviceType).deviceClass;
	}

	if (!mDevices.contains(deviceType)) {
		throw MalformedConfigException(QString("Port '%1' is configured to use unknown device class '%2'")
				.arg(port)
				.arg(deviceType));
	}

	return deviceType;
}

QStringList Configurer::initScripts() const
{
	return mInitScripts;
}

void Configurer::parseDeviceClasses(QDomElement const &element)
{
	QDomNodeList const deviceClasses = element.childNodes();
	for (int i = 0; i < deviceClasses.size(); ++i) {
		QDomElement const deviceNode = deviceClasses.item(i).toElement();
		if (!deviceNode.isNull()) {
			Device device;
			device.name = deviceNode.tagName();
			device.isOptional = deviceNode.attribute("optional", "false") == "true";
			QDomNamedNodeMap const &attributes = deviceNode.attributes();
			for (QDomNamedNodeMapLengthType j = 0; j < attributes.length(); ++j) {
				QDomAttr const &attribute = attributes.item(j).toAttr();
				device.attributes.insert(attribute.name(), attribute.value());
			}

			mDevices.insert(device.name, device);
		}
	}
}

void Configurer::parseDevicePorts(QDomElement const &element)
{
	QDomNodeList const devicePorts = element.childNodes();
	for (int i = 0; i < devicePorts.size(); ++i) {
		QDomElement const devicePortNode = devicePorts.item(i).toElement();
		if (!devicePortNode.isNull()) {
			QString const deviceName = devicePortNode.tagName();
			if (!mDevices.contains(deviceName)) {
				throw MalformedConfigException("Device is not listed in 'DeviceClasses' section", devicePortNode);
			}

			QString const port = devicePortNode.attribute("port");
			if (port.isEmpty()) {
				throw MalformedConfigException("Port map shall have non-empty 'port' attribute", devicePortNode);
			}

			QDomNamedNodeMap const &attributes = devicePortNode.attributes();
			for (QDomNamedNodeMapLengthType j = 0; j < attributes.length(); ++j) {
				QDomAttr const &attribute = attributes.item(j).toAttr();
				mDevices[deviceName].portSpecificAttributes[port].insert(attribute.name(), attribute.value());
			}
		}
	}
}

void Configurer::parseDeviceTypes(QDomElement const &element)
{
	QDomNodeList const deviceTypes = element.childNodes();
	for (int i = 0; i < deviceTypes.size(); ++i) {
		QDomElement const deviceTypeNode = deviceTypes.item(i).toElement();
		if (!deviceTypeNode.isNull()) {
			DeviceType deviceType;
			deviceType.name = deviceTypeNode.tagName();
			deviceType.deviceClass = deviceTypeNode.attribute("class");
			if (deviceType.deviceClass.isEmpty()) {
				throw MalformedConfigException("Device type shall have 'class' attribute", deviceTypeNode);
			}

			if (!mDevices.contains(deviceType.deviceClass)) {
				throw MalformedConfigException("Device is not listed in 'DeviceClasses' section", deviceTypeNode);
			}

			QDomNamedNodeMap const &attributes = deviceTypeNode.attributes();
			for (QDomNamedNodeMapLengthType j = 0; j < attributes.length(); ++j) {
				QDomAttr const &attribute = attributes.item(j).toAttr();
				deviceType.attributes.insert(attribute.name(), attribute.value());
			}

			mDeviceTypes.insert(deviceType.name, deviceType);
		}
	}
}

void Configurer::parseInitScript(QDomElement const &element)
{
	mInitScripts.append(element.text());
}

void Configurer::parseAdditionalConfigurations(QDomElement const &element)
{
	QDomNodeList const tags = element.childNodes();
	for (int i = 0; i < tags.size(); ++i) {
		QDomElement const tag = tags.item(i).toElement();
		if (!tag.isNull()) {
			if (tag.tagName() == "initScript"
					|| tag.tagName() == "deviceClasses"
					|| tag.tagName() == "devicePorts"
					|| tag.tagName() == "deviceTypes")
			{
				continue;
			}

			Device additionalDevice;
			additionalDevice.name = tag.tagName();
			QDomNamedNodeMap const &attributes = tag.attributes();
			for (QDomNamedNodeMapLengthType j = 0; j < attributes.length(); ++j) {
				QDomAttr const &attribute = attributes.item(j).toAttr();
				additionalDevice.attributes.insert(attribute.name(), attribute.value());
			}

			mAdditionalConfiguration.insert(additionalDevice.name, additionalDevice);
		}
	}
}

void Configurer::parseModelConfig(QDomElement const &element)
{
	QDomNodeList const tags = element.childNodes();
	for (int i = 0; i < tags.size(); ++i) {
		QDomElement const tag = tags.item(i).toElement();
		if (!tag.isNull()) {
			if (tag.tagName() == "initScript") {
				parseInitScript(tag);
			} else if (tag.hasChildNodes()) {
				ModelConfigurationElement port;
				port.port = tag.tagName();
				QDomNodeList const devices = tag.childNodes();
				if (devices.count() > 1) {
					throw MalformedConfigException("Only one device can be configured on a port", tag);
				}

				QDomElement const device = devices.item(0).toElement();
				if (!device.isNull()) {
					port.deviceType = device.tagName();
					QDomNamedNodeMap const &attributes = tag.attributes();
					for (QDomNamedNodeMapLengthType j = 0; j < attributes.length(); ++j) {
						QDomAttr const &attribute = attributes.item(j).toAttr();
						port.attributes.insert(attribute.name(), attribute.value());
					}

					mModelConfiguration.insert(port.port, port);
				}
			} else {
				AdditionalModelConfigurationElement element;
				element.deviceType = tag.tagName();
				if (!mDevices.contains(element.deviceType)) {
					throw MalformedConfigException(
							"Device shall be listed in 'deviceClasses' section in system config", tag);
				}

				if (tag.attribute("disabled", "false") == "false") {
					QDomNamedNodeMap const &attributes = tag.attributes();
					for (QDomNamedNodeMapLengthType j = 0; j < attributes.length(); ++j) {
						QDomAttr const &attribute = attributes.item(j).toAttr();
						element.attributes.insert(attribute.name(), attribute.value());
					}

					mAdditionalModelConfiguration.insert(element.deviceType, element);
				}
			}
		}
	}
}
