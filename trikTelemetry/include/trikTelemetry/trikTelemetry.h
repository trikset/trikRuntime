/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include <trikNetwork/trikServer.h>

namespace trikControl {
class BrickInterface;
}

namespace trikTelemetry {

class Connection;

/// TrikTelemetry server provides an interface for getting information about ports configuration and sensors data
/// of a brick.
/// TrikTelemetry class creates for each client a new Connection which runs in a separate thread
/// and serves clients' requests for ports and sensors data.
class TrikTelemetry : public trikNetwork::TrikServer
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - a Brick used to respond to clients
	TrikTelemetry(trikControl::BrickInterface &brick);

private:
	Connection *connectionFactory();

	/// A Brick which is used by Connections to respond to clients' requests
	trikControl::BrickInterface &mBrick;
};

}
