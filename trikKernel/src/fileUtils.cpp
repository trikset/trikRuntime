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
#include <QtCore/QDebug>

#include <QsLog.h>

#include "exceptions/failedToOpenFileException.h"
#include "exceptions/failedToParseXmlException.h"

using namespace trikKernel;

QString FileUtils::readFromFile(QString const &fileName)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!file.isOpen()) {
		throw FailedToOpenFileException(file);
	}

	QTextStream input;
	input.setDevice(&file);
	input.setCodec("UTF-8");
	QString const result = input.readAll();
	file.close();

	return result;
}

void FileUtils::writeToFile(QString const &fileName, QString const &contents, QString const &dirPath)
{
	QDir dir;
	dir.mkdir(dirPath);

	QString const filePath = dirPath.isEmpty() ? fileName : QString(dirPath + "/" + fileName);

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

QDomElement FileUtils::readXmlFile(QString const &path, QString const &fileName)
{
	QDomDocument document("file");
	QString correctedPath = path.endsWith(QDir::separator()) ? path : path + QDir::separator();

	QFile file(correctedPath + fileName);
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

