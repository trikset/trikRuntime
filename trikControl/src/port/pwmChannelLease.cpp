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

#include "pwmChannelLease.h"

#include <utility>

#include <QtCore/QFileInfo>
#include <QtCore/QScopedPointer>

#include <QsLog.h>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/outputDeviceFileInterface.h>

#include "pwmChipResolver.h"

using namespace trikControl;

PwmChannelLease::PwmChannelLease(const QString &exportSpec,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mHardwareAbstraction(&hardwareAbstraction)
{
	const auto fail = [this] { clear(); };

	auto line = 0;
	const auto exportFile = findPwmExportFile(exportSpec, line);
	if (exportFile.isEmpty()) {
		fail();
		return;
	}

	mChipDir = QFileInfo(exportFile).absolutePath();
	mLine = line;

	if (!QFileInfo::exists(channelDir())) {
		QScopedPointer<trikHal::OutputDeviceFileInterface> file(
			hardwareAbstraction.createOutputDeviceFile(exportFile));
		if (!file || !file->open()) {
			fail();
			return;
		}

		file->write(QString::number(mLine));
		file->close();
		mExportedByUs = true;
	}
}

PwmChannelLease::PwmChannelLease(PwmChannelLease &&other) noexcept
	: mHardwareAbstraction(other.mHardwareAbstraction)
	, mChipDir(std::move(other.mChipDir))
	, mLine(other.mLine)
	, mExportedByUs(other.mExportedByUs)
{
	other.clear();
}

PwmChannelLease &PwmChannelLease::operator=(PwmChannelLease &&other) noexcept
{
	if (this == &other) {
		return *this;
	}

	release();

	mHardwareAbstraction = other.mHardwareAbstraction;
	mChipDir = std::move(other.mChipDir);
	mLine = other.mLine;
	mExportedByUs = other.mExportedByUs;

	other.clear();

	return *this;
}

PwmChannelLease::~PwmChannelLease()
{
	release();
}

PwmChannelLease::operator bool() const
{
	return mHardwareAbstraction != nullptr && !mChipDir.isEmpty() && mLine >= 0;
}

void PwmChannelLease::release()
{
	unexport();
	clear();
}

void PwmChannelLease::clear()
{
	mHardwareAbstraction = nullptr;
	mChipDir.clear();
	mLine = -1;
	mExportedByUs = false;
}

int PwmChannelLease::line() const
{
	return mLine;
}

QString PwmChannelLease::channelDir() const
{
	if (mChipDir.isEmpty() || mLine < 0) {
		return {};
	}

	return mChipDir + QStringLiteral("/pwm") + QString::number(mLine);
}

void PwmChannelLease::unexport()
{
	if (!mExportedByUs || mHardwareAbstraction == nullptr) {
		return;
	}

	QScopedPointer<trikHal::OutputDeviceFileInterface> file(
		mHardwareAbstraction->createOutputDeviceFile(mChipDir + QStringLiteral("/unexport")));
	if (!file || !file->open()) {
		QLOG_WARN() << "PwmChannelLease: cannot unexport PWM channel" << mLine << "of" << mChipDir;
	} else {
		file->write(QString::number(mLine));
		file->close();
	}

	mExportedByUs = false;
}
