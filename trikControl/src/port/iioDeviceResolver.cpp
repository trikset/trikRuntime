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

#include "iioDeviceResolver.h"

#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

namespace trikControl {

namespace {

const auto iioDevicesPath = QStringLiteral("/sys/bus/iio/devices");
const auto iioDeviceFilter = QStringLiteral("iio:device*");

/// Reads a small text sysfs attribute and trims the trailing newline.
QString readAttribute(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return {};
	}

	return QString::fromUtf8(file.readAll()).trimmed();
}

/// Returns the device-tree node directory exposed for @p deviceDir.
///
/// An IIO device exposes it directly as "of_node"; when the IIO device is a
/// child of a platform device it is available in "device/of_node".
QString ofNodePath(const QString &deviceDir)
{
	auto ownOfNode = deviceDir + QStringLiteral("/of_node");
	if (QFileInfo::exists(ownOfNode)) {
		return ownOfNode;
	}

	const auto parentOfNode = deviceDir + QStringLiteral("/device/of_node");
	return QFileInfo::exists(parentOfNode) ? parentOfNode : QString();
}

/// Returns the index of @p channelName in the DT "channels" string array of
/// the IIO device, or -1 when the property or the name is not present.
int dtChannelIndex(const QString &deviceDir, const QString &channelName)
{
	const auto ofNode = ofNodePath(deviceDir);
	if (ofNode.isEmpty()) {
		return -1;
	}

	QFile file(ofNode + QStringLiteral("/channels"));
	if (!file.open(QIODevice::ReadOnly)) {
		return -1;
	}

	const auto entries = file.readAll().split('\0');
	for (auto i = 0; i < entries.size(); ++i) {
		if (QString::fromUtf8(entries.at(i)) == channelName) {
			return i;
		}
	}

	return -1;
}

/// Returns @p deviceDir/@p channel when such a file exists.
QString directChannelFile(const QString &deviceDir, const QString &channel)
{
	const auto channelFile = deviceDir + QLatin1Char('/') + channel;
	return QFileInfo::exists(channelFile) ? channelFile : QString();
}

} // namespace

QString findIioDeviceByLabel(const QString &label)
{
	if (label.isEmpty()) {
		return {};
	}

	const auto iioDevices = QDir(iioDevicesPath);
	const auto devices = iioDevices.entryList({iioDeviceFilter}, QDir::Dirs | QDir::NoDotAndDotDot);

	for (const auto &device : devices) {
		auto deviceDir = iioDevices.filePath(device);
		if (readAttribute(deviceDir + QStringLiteral("/label")) == label) {
			return deviceDir;
		}
	}

	return {};
}

QString findIioChannelFileByLabel(const QString &label, const QString &channel)
{
	if (channel.isEmpty()) {
		return {};
	}

	const auto deviceDir = findIioDeviceByLabel(label);
	if (deviceDir.isEmpty()) {
		return {};
	}

	auto direct = directChannelFile(deviceDir, channel);
	if (!direct.isEmpty()) {
		return direct;
	}

	const auto index = dtChannelIndex(deviceDir, channel);
	if (index < 0) {
		return {};
	}

	const auto mapped = deviceDir + QStringLiteral("/in_voltage") + QString::number(index) + QStringLiteral("_raw");
	return QFileInfo::exists(mapped) ? mapped : QString();
}

} // namespace trikControl
