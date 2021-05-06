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

#include <QtCore/QDir>
#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QImage>
#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Interface for camera implementations
class TRIKCONTROL_EXPORT CameraImplementationInterface
{
	Q_DISABLE_COPY(CameraImplementationInterface)
public:

	/// Get photo as a vector of uint8t in RGB 888 format
	virtual QVector<uint8_t> getPhoto() = 0;

	virtual ~CameraImplementationInterface() = default;

	CameraImplementationInterface() = default;

	/// Get directory, where photos are saved
	QDir getTempDir() const { return tempDir;}

	/// Set directory, where photos are saved
	/// @param newDir - new name of tempDir
	void setTempDir(const QDir &newDir) {tempDir = newDir;}

private:
	QDir tempDir;

};

}
