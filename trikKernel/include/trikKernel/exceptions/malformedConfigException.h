/* Copyright 2015 CyberTech Labs Ltd.
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
#include <QtXml/QDomNode>

#include <QsLog.h>

#include "trikRuntimeException.h"

namespace trikKernel {

/// Exception that is thrown when something is wrong with runtime configuration files.
class TRIKKERNEL_EXPORT MalformedConfigException : public TrikRuntimeException
{
public:
	/// Constructor.
	/// @param errorMessage - string message that specifies what was wrong.
	explicit MalformedConfigException(const QString &errorMessage)
		: TrikRuntimeException(QString("Configuration file malformed: %1").arg(errorMessage))
		, mErrorMessage(errorMessage)
	{
	}

	/// Constructor.
	/// @param errorMessage - string message that specifies what was wrong.
	/// @param node - XML node where error was detected.
	MalformedConfigException(const QString &errorMessage, const QDomNode &node)
		: MalformedConfigException(QString("Configuration file malformed, line %1, column %2: %3")
				.arg(node.lineNumber()).arg(node.columnNumber()).arg(errorMessage))
	{
	}

	/// Returns error message.
	QString errorMessage() const
	{
		return mErrorMessage;
	}

private:
	const QString mErrorMessage;
};

}
