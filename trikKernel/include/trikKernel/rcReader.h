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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QFile>
#include "trikKernelDeclSpec.h"

namespace trikKernel {

/// Simple trikrc file parser.
class TRIKKERNEL_EXPORT RcReader : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param rcFilePath - trikrc file path.
	/// @param parent - parent of this object in Qt object hierarchy.
	explicit RcReader(const QString &rcFilePath, QObject *parent = 0);

	/// Clears data from previous trikrc reading and reads it again.
	void read();

	/// Get value of variable in trikrc.
	/// @param name - variable name.
	/// @return value if variable is found and empty string otherwise.
	QString value(const QString &name) const;

private:
	QHash<QString, QString> mVariables;
	QFile mRcFile;
};

}
