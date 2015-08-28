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

#include "brickFactory.h"

#include <QtCore/QDir>

#include <trikKernel/fileUtils.h>

#include "brick.h"

using namespace trikControl;

BrickInterface *BrickFactory::create(const QString &systemConfig
		, const QString &modelConfig, const QString &mediaPath)
{
	return new Brick(systemConfig, modelConfig, mediaPath);
}

BrickInterface *BrickFactory::create(const QString &configFilesPath, const QString &mediaPath)
{
	const QString correctedPath = trikKernel::FileUtils::normalizePath(configFilesPath);
	return new Brick(correctedPath + "system-config.xml", correctedPath + "model-config.xml", mediaPath);
}

BrickInterface *BrickFactory::create(
		trikHal::HardwareAbstractionInterface &hardwareAbstraction
		, const QString &systemConfig
		, const QString &modelConfig
		, const QString &mediaPath)
{
	return new Brick(hardwareAbstraction, systemConfig, modelConfig, mediaPath);
}

