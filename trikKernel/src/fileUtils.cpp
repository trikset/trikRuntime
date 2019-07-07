/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "fileUtils.h"

#include <QtCore/QFile>
#include <QtCore/QDir>

#include "exceptions/failedToOpenFileException.h"
#include "exceptions/failedToParseXmlException.h"

using namespace trikKernel;

QString FileUtils::readFromFile(const QString &fileName)
{
	QFile file(fileName);
	auto opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!opened) {
		throw FailedToOpenFileException(file);
	}

	QTextStream input;
	input.setDevice(&file);
	input.setCodec("UTF-8");
	const QString result = input.readAll();
	file.close();

	return result;
}

void FileUtils::writeToFile(const QString &fileName, const QString &contents, const QString &dirPath)
{
	QDir dir;
	dir.mkdir(dirPath);

	const QString normalizedDirPath = normalizePath(dirPath);

	const QString filePath = dirPath.isEmpty() ? fileName : QString(normalizedDirPath + fileName);

	QFile file(filePath);
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	if (!file.isOpen()) {
		throw FailedToOpenFileException(file);
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << contents;
	file.close();
}

QDomElement FileUtils::readXmlFile(const QString &fileNameWithPath)
{
	QDomDocument document("file");
	QFile file(fileNameWithPath);
	if (!file.open(QIODevice::ReadOnly)) {
		throw FailedToOpenFileException(file);
	}

	int errorLine = 0;
	int errorColumn = 0;
	QString errorMessage;
	if (!document.setContent(&file, &errorMessage, &errorLine, &errorColumn)) {
		file.close();
		throw FailedToParseXmlException(file, errorMessage, errorLine, errorColumn);
	}

	file.close();

	return document.documentElement();
}

QString FileUtils::normalizePath(const QString &path)
{
	QString result = QDir(path).absolutePath();
	if (!result.endsWith(QDir::separator())) {
		result += QDir::separator();
	}

	return result;
}
