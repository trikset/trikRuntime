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

#include <QtCore/QFile>
#include <QtCore/QString>

#include <QsLog.h>

#include "trikRuntimeException.h"

namespace trikKernel {

/// Exception that is thrown when XML file is corrupt and "setContent" method of QDomDocument has failed.
class TRIKKERNEL_EXPORT FailedToParseXmlException : public TrikRuntimeException
{
public:
	/// Constructor.
	/// @param file - XML file that is failed to parse.
	/// @param errorMessage - error message from QDomDocument::setContent.
	/// @param errorLine - line in XML document where error occured.
	/// @param errorColumn - column in XML document where error occured.
	FailedToParseXmlException(const QFile &file, const QString &errorMessage, int errorLine, int errorColumn)
		: TrikRuntimeException(QString("Failed to parse %1, %2:%3 - %4")
							   .arg(file.fileName())
							   .arg(errorLine)
							   .arg(errorColumn)
							   .arg(errorMessage))
		, mFile(file), mErrorMessage(errorMessage), mErrorLine(errorLine), mErrorColumn(errorColumn)
	{
	}

	/// Returns XML file that is failed to parse.
	const QFile &file() const
	{
		return mFile;
	}

	/// Returns error message.
	QString errorMessage() const
	{
		return mErrorMessage;
	}

	/// Returns line in XML document where error occured.
	int errorLine() const
	{
		return mErrorLine;
	}

	/// Returns column in XML document where error occured.
	int errorColumn() const
	{
		return mErrorColumn;
	}

private:
	const QFile &mFile;
	const QString mErrorMessage;
	const int mErrorLine;
	const int mErrorColumn;
};

}
