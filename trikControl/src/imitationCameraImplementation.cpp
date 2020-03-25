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

#include "imitationCameraImplementation.h"
#include "cameraDeviceInterface.h"

#include <QtCore/QDir>
#include <QtGui/QImage>

#include <QsLog.h>

using namespace trikControl;

ImitationCameraImplementation::ImitationCameraImplementation(const QStringList &filter, const QString &path)
	: mFilters(filter)
{
	setTempDir(path);
	mFiles = QDir(getTempDir()).entryInfoList(mFilters
			, QDir::Filter::NoDotAndDotDot | QDir::Filter::Readable | QDir::Filter::Files
			, QDir::SortFlag::Name | QDir::SortFlag::IgnoreCase);
	if (mFiles.isEmpty()) {
		QLOG_WARN() << "No files in directory " << getTempDir()
				<< " matching" << mFilters;
	} else {
		QLOG_INFO() << "Few files matching regexps " << mFilters
				<< " in file " << getTempDir();
		if (mFiles.size() < 10) {
			for (auto &&file : mFiles) {
				QLOG_INFO() << file.fileName();
			}
		}
	}
}


QVector<uint8_t> ImitationCameraImplementation::getPhoto() {
	if (mFiles.isEmpty()) {
		return QVector<uint8_t>();
	}
	auto const &path = mFiles[++mCurrentFileIndex %= mFiles.size()].absoluteFilePath();
	QImage imgOrig(path);

	if (!imgOrig.isNull()) {
		QLOG_INFO() << "getPhoto: using " << path;
	} else {
		QLOG_ERROR() << "getPhoto: can not open file " << path;
		return QVector<uint8_t>();
	}

	return CameraDeviceInterface::qImageToQVector(imgOrig);
}
