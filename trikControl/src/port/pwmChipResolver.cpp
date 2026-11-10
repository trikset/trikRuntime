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

#include "pwmChipResolver.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

namespace trikControl {

namespace {

/// Returns the canonical path of the platform device behind @p chipDir, or an
/// empty string when the pwmchip has no device link or it cannot be resolved.
QString chipDevicePath(const QString &chipDir)
{
	return QFileInfo(chipDir + QStringLiteral("/device")).canonicalFilePath();
}

/// Returns true when @p deviceName identifies the device at @p devicePath.
///
/// @p deviceName is either a bare platform device name ("fe700020.pwm") or a
/// full path to it.
bool deviceMatches(const QString &deviceName, const QString &devicePath)
{
	if (devicePath.isEmpty()) {
		return false;
	}

	if (deviceName.contains(QLatin1Char('/'))) {
		return devicePath == QFileInfo(deviceName).canonicalFilePath();
	}

	return QFileInfo(devicePath).fileName() == deviceName;
}

/// Resolves an explicit sysfs-like path without scanning /sys/class/pwm.
///
/// Accepted forms:
///   - "/path/to/pwmchipN"      - chip directory;
///   - "/path/to/pwmchipN/pwmM" - channel directory, M updates @p line;
///   - "/path/to/export"        - export file.
///
/// This form is used by tests and for bring-up, where the sysfs tree may not
/// exist yet. Returns an empty string for anything else.
QString explicitExportFile(const QString &path, int &line)
{
	if (!path.contains(QLatin1Char('/'))) {
		return {};
	}

	const QFileInfo info(path);
	const auto name = info.fileName();

	if (name == QStringLiteral("export")) {
		return path;
	}

	if (name.startsWith(QStringLiteral("pwmchip"))) {
		return path + QStringLiteral("/export");
	}

	if (name.startsWith(QStringLiteral("pwm"))) {
		auto ok = false;
		const auto parsed = name.mid(3).toInt(&ok);
		if (ok && parsed >= 0) {
			line = parsed;
			return info.absolutePath() + QStringLiteral("/export");
		}
	}

	return {};
}

} // namespace

QString findPwmExportFile(const QString &exportSpec, int &line)
{
	line = 0;

	const auto spec = exportSpec.trimmed();
	if (spec.isEmpty()) {
		return {};
	}

	auto device = spec;
	const auto separator = spec.lastIndexOf(QLatin1Char(':'));
	if (separator > 0) {
		auto ok = false;
		const auto parsed = spec.mid(separator + 1).toInt(&ok);
		if (!ok || parsed < 0) {
			return {};
		}

		device = spec.left(separator);
		line = parsed;
	}

	auto explicitFile = explicitExportFile(device, line);
	if (!explicitFile.isEmpty()) {
		return explicitFile;
	}

	const auto chipDir = findPwmChipByDevice(device);
	return chipDir.isEmpty() ? QString() : chipDir + QStringLiteral("/export");
}

QString findPwmChipByDevice(const QString &deviceName)
{
	if (deviceName.isEmpty()) {
		return {};
	}

	const auto pwmClass = QDir(QStringLiteral("/sys/class/pwm"));
	const auto chips = pwmClass.entryList({QStringLiteral("pwmchip*")}, QDir::Dirs | QDir::NoDotAndDotDot);

	for (const auto &chipName : chips) {
		auto chipDir = pwmClass.filePath(chipName);
		if (deviceMatches(deviceName, chipDevicePath(chipDir))) {
			return chipDir;
		}
	}

	return {};
}

} // namespace trikControl
