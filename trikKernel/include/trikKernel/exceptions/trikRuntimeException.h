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

#include <exception>
#include <QString>
#include <QsLog.h>
#include <trikKernel/trikKernelDeclSpec.h>

namespace trikKernel {

/// Base class for all exceptions in TRIKRuntime.
class TrikRuntimeException : public std::exception
{
public:
	/// Constructor
	/// @param msg - message to be returned
	explicit TrikRuntimeException(const QString &msg):
		mMessage(msg)
	{
		/// Vera
		QLOG_ERROR() << message();
	}

	/// Destructor
	~TrikRuntimeException() = default;

	/// Returns the exception message
	const char *what() const noexcept override
	{
		/// Vera
		return message().toLatin1().constData();
	}

	/// Returns the exception message
	const QString &message() const { return mMessage; }
private:
	QString mMessage;
};

}
