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

#include "gpioLineLease.h"

#include <algorithm>
#include <utility>

#include <QtCore/QFileInfo>
#include <QtCore/QScopedPointer>

#include <QsLog.h>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/outputDeviceFileInterface.h>

namespace trikControl {

namespace {

const auto gpioClass = QStringLiteral("/sys/class/gpio/");
const auto gpioPrefix = QStringLiteral("/sys/class/gpio/gpio");

/// Returns true when @p value consists of decimal digits only and is not empty.
bool isDigits(const QString &value)
{
	return !value.isEmpty() && std::all_of(value.cbegin(), value.cend(), [](QChar ch) { return ch.isDigit(); });
}

/// Converts an RK GPIO name ("GPIO4_C5") to the global sysfs GPIO number.
/// @return the global number, or -1 when @p name is malformed.
int rkGpioNumber(const QString &name)
{
	if (!name.startsWith(QStringLiteral("GPIO"))) {
		return -1;
	}

	const auto separator = name.indexOf(QLatin1Char('_'));
	if (separator <= 4) {
		return -1;
	}

	auto bankOk = false;
	const auto bank = name.mid(4, separator - 4).toInt(&bankOk);
	if (!bankOk || bank < 0 || bank > 4) {
		return -1;
	}

	const auto pin = name.mid(separator + 1);
	if (pin.size() != 2 || pin[0] < QLatin1Char('A') || pin[0] > QLatin1Char('D') || !pin[1].isDigit()) {
		return -1;
	}

	const auto group = pin[0].unicode() - QLatin1Char('A').unicode();
	const auto index = pin[1].digitValue();
	if (group < 0 || group > 3 || index < 0 || index > 7) {
		return -1;
	}

	return bank * 32 + group * 8 + index;
}

/// Extracts the GPIO number from a sysfs value file path.
/// @return the global number, or -1 when @p valueFile is not a sysfs GPIO
///         value file.
int gpioNumber(const QString &valueFile)
{
	const auto info = QFileInfo(valueFile);
	if (info.fileName() != QStringLiteral("value")) {
		return -1;
	}

	const auto dir = info.absolutePath();
	if (!dir.startsWith(gpioPrefix)) {
		return -1;
	}

	const auto number = dir.mid(gpioPrefix.size());
	if (!isDigits(number)) {
		return -1;
	}

	auto ok = false;
	const auto result = number.toInt(&ok);
	return ok ? result : -1;
}

/// Writes @p value to @p fileName through the HAL.
bool writeFile(const trikHal::HardwareAbstractionInterface &hardwareAbstraction, const QString &fileName,
	const QString &value)
{
	QScopedPointer<trikHal::OutputDeviceFileInterface> file(hardwareAbstraction.createOutputDeviceFile(fileName));
	if (!file || !file->open()) {
		return false;
	}

	file->write(value);
	file->close();
	return true;
}

} // namespace

GpioLineLease::GpioLineLease(const QString &valueFile, Direction direction, bool initialValue,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
{
	auto number = -1;
	if (valueFile.startsWith(QStringLiteral("GPIO"))) {
		number = rkGpioNumber(valueFile);
		if (number < 0) {
			QLOG_ERROR() << "GpioLineLease: invalid RK GPIO name" << valueFile;
			return;
		}
	} else {
		number = gpioNumber(valueFile);
		if (number < 0) {
			// Not a sysfs GPIO value file (for example a test fixture path).
			mValueFile = valueFile;
			mValid = true;
			return;
		}
	}

	mHardwareAbstraction = &hardwareAbstraction;
	mNumber = QString::number(number);

	const auto lineDir = gpioClass + QStringLiteral("gpio") + mNumber;
	mValueFile = lineDir + QStringLiteral("/value");
	if (!QFileInfo::exists(lineDir)) {
		if (!writeFile(hardwareAbstraction, gpioClass + QStringLiteral("export"), mNumber)) {
			QLOG_ERROR() << "GpioLineLease: cannot export GPIO" << mNumber;
			return;
		}

		mExportedByUs = true;
	}

	const auto directionValue = direction == Direction::Out ? QStringLiteral("out") : QStringLiteral("in");
	if (!writeFile(hardwareAbstraction, lineDir + QStringLiteral("/direction"), directionValue)) {
		QLOG_ERROR() << "GpioLineLease: cannot set direction of GPIO" << mNumber;
		unexport();
		return;
	}

	if (direction == Direction::Out) {
		const auto value = initialValue ? QStringLiteral("1") : QStringLiteral("0");
		if (!writeFile(hardwareAbstraction, lineDir + QStringLiteral("/value"), value)) {
			QLOG_ERROR() << "GpioLineLease: cannot set value of GPIO" << mNumber;
			unexport();
			return;
		}
	}

	mValid = true;
}

GpioLineLease::GpioLineLease(GpioLineLease &&other) noexcept
	: mHardwareAbstraction(other.mHardwareAbstraction)
	, mNumber(std::move(other.mNumber))
	, mValueFile(std::move(other.mValueFile))
	, mExportedByUs(other.mExportedByUs)
	, mValid(other.mValid)
{
	other.clear();
}

GpioLineLease &GpioLineLease::operator=(GpioLineLease &&other) noexcept
{
	if (this == &other) {
		return *this;
	}

	release();

	mHardwareAbstraction = other.mHardwareAbstraction;
	mNumber = std::move(other.mNumber);
	mValueFile = std::move(other.mValueFile);
	mExportedByUs = other.mExportedByUs;
	mValid = other.mValid;

	other.clear();

	return *this;
}

GpioLineLease::~GpioLineLease()
{
	release();
}

GpioLineLease::operator bool() const
{
	return mValid;
}

QString GpioLineLease::valueFile() const
{
	return mValueFile;
}

void GpioLineLease::release()
{
	unexport();
	clear();
}

void GpioLineLease::clear()
{
	mHardwareAbstraction = nullptr;
	mNumber.clear();
	mValueFile.clear();
	mExportedByUs = false;
	mValid = false;
}

void GpioLineLease::unexport()
{
	if (!mExportedByUs || mHardwareAbstraction == nullptr) {
		return;
	}

	writeFile(*mHardwareAbstraction, gpioClass + QStringLiteral("unexport"), mNumber);
	mExportedByUs = false;
}

} // namespace trikControl
