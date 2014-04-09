/* Copyright 2014 CyberTech Labs Ltd.
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

#include "rcReader.h"

#include <QtCore/QDebug>

RcReader::RcReader(const QString &rcFilePath, QObject *parent)
	: QObject(parent)
	, mRcFile(rcFilePath)
{
	read();
}

void RcReader::read()
{
	mVariables.clear();

	if (!mRcFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Can't open trikrc file";
		return;
	}

	forever {
		QString line = mRcFile.readLine();
		if (line.isEmpty()) {
			break;
		}

		int const commentStart = line.indexOf('#');
		if (commentStart >= 0) {
			line.truncate(commentStart);
		}

		int const equalsSign = line.indexOf('=');
		if (equalsSign < 0) {
			continue;
		}

		QString const name = line.left(equalsSign);
		QString const value = line.mid(equalsSign + 1);

		mVariables[name] = value;
	}
}

QString RcReader::value(const QString &name)
{
	return mVariables.value(name);
}
