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

#include <QtCore/QDir>
#include <QtGui/QImage>

#include <QsLog.h>

using namespace trikControl;

ImitationCameraImplementation::ImitationCameraImplementation(const QStringList &filter, const QString &path)
	: filters(filter)
{
	setTempDir(path);
	filesList = QDir(getTempDir()).entryInfoList(filters);
	if (filesList.isEmpty()) {
		QLOG_ERROR() << "No files in directory " << getTempDir()
				<< " matching regexps " << filters;
	}

	if (filesList.size() > 1) {
		QLOG_INFO() << "Few files matching regexps " << filters
				<< " in file " << getTempDir();

		if (filesList.size() < 10) {
			for (auto &&file : filesList) {
				QLOG_INFO() << file.fileName();
			}
		}
	}
}


QVector<uint8_t> ImitationCameraImplementation::getPhoto() {
	if ( ! filesList.isEmpty()) {
		auto f = filesList[++cur%=filesList.size()];
		QImage imgOrig(f.absoluteFilePath());

		if (! imgOrig.isNull()) {
			QLOG_INFO() << "Opening file " << f.absoluteFilePath();
		} else {
			QLOG_ERROR() << "Can not open file " << f.absoluteFilePath();
			return QVector<uint8_t>();
		}

		return qImageToQVector(imgOrig);
	} else {
		QLOG_INFO() << "Return empty image, are filters for camera correct?";
		return QVector<uint8_t>();
	}
}
