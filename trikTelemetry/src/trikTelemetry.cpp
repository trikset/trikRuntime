#include "trikTelemetry.h"

#include "src/connection.h"

namespace trikTelemetry {

TrikTelemetry::TrikTelemetry(trikControl::Brick &brick)
	: trikKernel::TrikServer([this] () { return connectionFactory(); })
	, mBrick(brick)
{
}

Connection * TrikTelemetry::connectionFactory()
{
	return new Connection(mBrick);
}

}
