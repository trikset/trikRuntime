#pragma once

#include <trikKernel/trikServer.h>
#include <trikControl/brick.h>

namespace trikTelemetry {

class Connection;

/// TrikTelemetry server provides an interface for getting information about ports configuration and sensors data
/// of a brick.
/// TrikTelemetry class creates for each client a new Connection which runs in a separate thread
/// and serves clients' requests for ports and sensors data.
class TrikTelemetry : public trikKernel::TrikServer
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - a Brick used to respond to clients
	TrikTelemetry(trikControl::Brick &brick);

private:
	Connection *connectionFactory();

	/// A Brick which is used by Connections to respond to clients' requests
	trikControl::Brick &mBrick;
};

}
