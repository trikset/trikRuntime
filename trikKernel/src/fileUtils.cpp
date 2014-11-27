/* Copyright 2013 - 2014 Yurii Litvinov, CyberTech Labs Ltd.
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

#include "QsLog.h"

using namespace trikKernel;

QString FileUtils::readFromFile(QString const &fileName)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!file.isOpen()) {
		qDebug() << "Failed to open file" << fileName << "for reading";
		QLOG_FATAL() << "Failed to open file" << fileName << "for reading";
		throw "Failed to open file";
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
		qDebug() << "Failed to open file" << filePath << "for writing";
		QLOG_FATAL() << "Failed to open file" << filePath << "for writing";
		throw "File open operation failed";
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << contents;
	file.close();
}
