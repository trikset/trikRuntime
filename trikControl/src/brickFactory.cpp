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

#include "brick.h"

using namespace trikControl;

BrickInterface *BrickFactory::create(QString const &systemConfig
		, QString const &modelConfig, QString const &startDirPath)
{
	return new Brick(systemConfig, modelConfig, startDirPath);
}

BrickInterface *BrickFactory::create(QString const &configFilesPath, QString const &startDirPath)
{
	QString const correctedPath = configFilesPath.endsWith(QDir::separator())
			? configFilesPath
			: configFilesPath + QDir::separator();

	return new Brick(correctedPath + "system-config.xml", correctedPath + "model-config.xml", startDirPath);
}

