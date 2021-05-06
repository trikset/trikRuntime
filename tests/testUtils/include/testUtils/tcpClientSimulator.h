/* Copyright 2016 Yurii Litvinov, CyberTech Labs Ltd.
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

#include <trikNetwork/connection.h>

#include "testUtilsDeclSpec.h"

namespace tests {
namespace utils {

/// Utility class that can simulate TCP network activity according to protocols used by TrikRuntime.
class TESTUTILS_EXPORT TcpClientSimulator : public trikNetwork::Connection
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param ip - ip addres of a server to which we shall connect.
	/// @param port - port of a server to which we shall connect.
	TcpClientSimulator(const QString &ip, int port);

	/// Returns latest received response from a server.
	QString latestResponse() const;

private:
	void processData(const QByteArray &data) override;

	/// Contains latest received response from a server.
	QString mLatestResponse;
};

}
}
