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
#include <QtCore/QThread>
#include <QtCore/QHash>
#include "trikKernelDeclSpec.h"

namespace trikKernel {

/// Debug helper that logs function entry and exit.
class TRIKKERNEL_EXPORT Debugger : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param methodName - name of a method to be logged
	Debugger(const QString &methodName);

	~Debugger() override;

private:
	const QString mMethodName;
	static QHash<QThread *, int> mIndent;
};

}

/// Macro to log method entry and exit.
#define L trikKernel::Debugger const debugger(Q_FUNC_INFO)
