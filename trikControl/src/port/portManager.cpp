/* Copyright 2026 CyberTech Labs Ltd.
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

#include "portManager.h"

#include <QtXml/QDomElement>
#include <QsLog.h>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/exceptions/malformedConfigException.h>
#include <trikKernel/fileUtils.h>

#include "hardwarePort.h"
#include "peripheryDriverInterface.h"
#include "peripheryPort.h"
#include "port.h"
#include "portLease.h"
#include "transports/transportType.h"

using namespace trikControl;

namespace {

PortMode modeFromString(const QString &name)
{
	if (name == QStringLiteral("pwm")) {
		return PortMode::Pwm;
	}

	if (name == QStringLiteral("uart")) {
		return PortMode::Uart;
	}

	if (name == QStringLiteral("gpio")) {
		return PortMode::Gpio;
	}

	if (name == QStringLiteral("i2c")) {
		return PortMode::I2c;
	}

	if (name == QStringLiteral("adc")) {
		return PortMode::Adc;
	}

	return PortMode::None;
}

QHash<PortMode, PortModeConfig> parseModes(const QDomElement &portElement)
{
	auto modes = QHash<PortMode, PortModeConfig>();
	for (auto element = portElement.firstChildElement(QStringLiteral("mode")); !element.isNull();
		element = element.nextSiblingElement(QStringLiteral("mode"))) {
		const auto name = element.attribute(QStringLiteral("name"));
		const auto mode = modeFromString(name);
		if (mode == PortMode::None) {
			QLOG_WARN() << "PortManager: unknown port mode" << name;
			continue;
		}

		if (!modes.contains(mode)) {
			PortModeConfig config;
			config.device = element.attribute(QStringLiteral("device"));

			auto busOk = false;
			const auto bus = element.attribute(QStringLiteral("bus")).toInt(&busOk);
			if (busOk && bus >= 0) {
				config.bus = bus;
			}

			modes.insert(mode, config);
		}
	}

	return modes;
}

} // namespace

PortManager::PortManager(const QString &systemConfig, trikHal::HardwareAbstractionInterface &hardwareAbstraction,
	PeripheryDriverInterface *driver)
	: mHardwareAbstraction(hardwareAbstraction)
	, mDriver(driver)
{
	parsePorts(systemConfig);
}

PortManager::~PortManager()
{
	qDeleteAll(mPorts);
}

PortLease PortManager::acquire(const QString &portName, PortMode mode, const QString &ownerName)
{
	auto *acquired = port(portName);
	if (!acquired) {
		QLOG_ERROR() << "PortManager: port" << portName << "is not configured";
		return {};
	}

	if (acquired->isActive() && acquired->activeMode() != mode) {
		QLOG_ERROR() << "PortManager: port" << portName << "is already occupied by" << acquired->owner()
			     << "in another mode";
		return {};
	}

	const auto token = acquired->start(mode, ownerName);
	if (token == 0) {
		QLOG_ERROR() << "PortManager: failed to acquire port" << portName << "for" << ownerName;
		return {};
	}

	return {acquired, token, acquired->activeDevice()};
}

bool PortManager::isConfigured(const QString &portName) const
{
	return mPorts.contains(portName);
}

PortLease PortManager::acquireI2c(int bus, const QString &ownerName)
{
	const auto portName = mI2cBusToPort.value(bus);
	if (portName.isEmpty()) {
		QLOG_ERROR() << "PortManager: no port configured for I2C bus" << bus;
		return {};
	}

	return acquire(portName, PortMode::I2c, ownerName);
}

Port *PortManager::port(const QString &name) const
{
	return mPorts.value(name, nullptr);
}

void PortManager::parsePorts(const QString &systemConfig)
{
	const auto root = trikKernel::FileUtils::readXmlFile(systemConfig);
	const auto sections = root.elementsByTagName(QStringLiteral("ports"));

	if (sections.isEmpty()) {
		QLOG_INFO() << "PortManager: no <ports> section in system config";
		return;
	}

	if (sections.size() > 1) {
		throw trikKernel::MalformedConfigException("'ports' element shall appear at most once in config");
	}

	const auto portsElement = sections.at(0).toElement();
	for (auto element = portsElement.firstChildElement(QStringLiteral("port")); !element.isNull();
		element = element.nextSiblingElement(QStringLiteral("port"))) {
		const auto name = element.attribute(QStringLiteral("name"));
		if (name.isEmpty()) {
			QLOG_ERROR() << "PortManager: <port> without a name";
			continue;
		}

		if (mPorts.contains(name)) {
			QLOG_WARN() << "PortManager: duplicate port" << name << "ignored";
			continue;
		}

		auto config = PortConfig {};
		config.name = name;
		config.modes = parseModes(element);

		auto *created = createPort(config);
		if (!created) {
			continue;
		}

		mPorts.insert(name, created);

		// Ports B5/B6 are wired to dedicated I2C buses. Such a port can be
		// configured either through model-config for the port or directly
		// through the Brick API (i2c()), hence the bus -> port mapping.
		const auto i2c = config.modes.constFind(PortMode::I2c);
		if (i2c != config.modes.cend() && i2c->bus >= 0) {
			if (mI2cBusToPort.contains(i2c->bus)) {
				QLOG_WARN() << "PortManager: I2C bus" << i2c->bus << "is assigned to both"
					    << mI2cBusToPort.value(i2c->bus) << "and" << name << "- keeping the first";
			} else {
				mI2cBusToPort.insert(i2c->bus, name);
			}
		}
	}
}

Port *PortManager::createPort(const PortConfig &config)
{
	const auto name = config.name;
	const auto allowedModes = config.modes.keys();

	if (parseTransport(name) == TransportType::Communicator) {
		if (!mDriver) {
			QLOG_WARN() << "PortManager: no periphery driver, skipping port" << name;
			return nullptr;
		}

		return new PeripheryPort(config, allowedModes, *mDriver);
	}

	return new HardwarePort(config, allowedModes, mHardwareAbstraction);
}
