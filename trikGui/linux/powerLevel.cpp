/* Copyright 2016 CyberTech Labs Ltd.
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

#include "powerLevel.h"

#include <QtCore/QString>

#include <trikKernel/fileUtils.h>
#include <trikKernel/exceptions/failedToOpenFileException.h>

#include <QsLog.h>

using namespace trikGui;

PowerLevel::Level PowerLevel::currentLevel()
{
	const QString powerLevelGpioPath = "/sys/class/gpio/gpio56/value";

	try {
		const QString currentValue = trikKernel::FileUtils::readFromFile(powerLevelGpioPath).trimmed();
		if (currentValue == "0") {
			QLOG_INFO() << "Current low power threshold is set to 6V";
			return Level::sixVolt;
		} else if (currentValue == "1") {
			QLOG_INFO() << "Current low power threshold is set to 12V";
			return Level::twelveVolt;
		} else {
			QLOG_ERROR() << "Invalid value in GPIO file '" << powerLevelGpioPath << "'. Value can be 0 or 1, but was '"
					<< currentValue << "'. Something is wrong with Linux kernel itself. Defaulting to 6V scheme.";
			return Level::sixVolt;
		}
	} catch (trikKernel::FailedToOpenFileException &e) {
		QLOG_ERROR() << "Power level GPIO file '" << powerLevelGpioPath << "' not found. It is not exported by kernel"
				" or file path is incorrect. Defaulting to 6V scheme.";
		return Level::sixVolt;
	}
}
