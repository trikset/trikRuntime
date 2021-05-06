/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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

#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QFileInfoList>

#include "cameraImplementationInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Class for camera imitation
class TRIKCONTROL_EXPORT ImitationCameraImplementation : public CameraImplementationInterface
{
public:

	/// Camera imitation constructor
	/// @param filter - list of filters to select photo from tempDir
	/// @param path - directory with prepared images
	ImitationCameraImplementation(const QStringList &filter, const QString &path);

	QVector<uint8_t> getPhoto() override;

	~ImitationCameraImplementation() override = default;

private:
	QStringList mFilters;
	QFileInfoList mFiles;
	int mCurrentFileIndex {-1};
};

}
