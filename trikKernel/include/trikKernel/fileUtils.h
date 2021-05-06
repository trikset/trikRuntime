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

#pragma once

#include <QtCore/QString>
#include <QtXml/QDomElement>
#include "trikKernelDeclSpec.h"

namespace trikKernel {

/// Helper class with file helper functions.
class TRIKKERNEL_EXPORT FileUtils
{
public:
	/// Reads all file contents and returns it as sting or throws an exception.
	static QString readFromFile(const QString &fileName);

	/// Writes given string to given file, throws exception if something went wrong.
	static void writeToFile(const QString &fileName, const QString &contents, const QString &dirPath = "");

	/// Reads given XML file and returns root element, throws exception if something went wrong.
	static QDomElement readXmlFile(const QString &fileNameWithPath);

	/// Returns absolute path by given path, adds "/" to the right if needed.
	static QString normalizePath(const QString &path);
};

}
