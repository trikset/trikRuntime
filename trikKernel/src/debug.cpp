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

#include "debug.h"

#include <QtCore/QDebug>

using namespace trikKernel;

QHash<QThread *, int> Debugger::mIndent;

Debugger::Debugger(const QString &methodName)
	: mMethodName(methodName)
{
	const QString indent(mIndent[thread()], ' ');
	const QString str = QString("%1(Thread %2) Entering %3")
			.arg(indent)
			.arg(reinterpret_cast<quintptr>(thread()), QT_POINTER_SIZE * 2, 16, QLatin1Char('0'))
			.arg(mMethodName);

	qDebug() << str.toStdString().c_str();
	++mIndent[thread()];
}

Debugger::~Debugger()
{
	--mIndent[thread()];
	const QString indent(mIndent[thread()], ' ');
	const QString str = QString("%1(Thread %2) Exiting %3")
			.arg(indent)
			.arg(reinterpret_cast<quintptr>(thread()), QT_POINTER_SIZE * 2, 16, QLatin1Char('0'))
			.arg(mMethodName);

	qDebug() << str.toStdString().c_str();
}
