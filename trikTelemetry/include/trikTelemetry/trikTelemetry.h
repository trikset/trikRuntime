#pragma once

#include <trikKernel/trikServer.h>
#include <trikControl/brick.h>

namespace trikTelemetry {

class Connection;

class TrikTelemetry : public trikKernel::TrikServer
{
	Q_OBJECT
public:
	TrikTelemetry(trikControl::Brick &brick);

private:
	Connection *connectionFactory();

	trikControl::Brick &mBrick;
};

}
