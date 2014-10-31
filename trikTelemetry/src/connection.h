#pragma once

#include <trikKernel/connection.h>
#include <trikControl/brick.h>

namespace trikTelemetry {

/// Connection class accepts requests for sensors configuration and current sensor values. Uses a brick
/// passed to it as a constructor parameter to respond to those requests.
///
/// Accepted commands:
///     data - sends data from sensors to a client
///     ports - sends current ports configuration to a client
class Connection : public trikKernel::Connection
{
	Q_OBJECT
public:
	Connection(trikControl::Brick &brick);

private:
	void processData(QByteArray const &data) override;

	static QString serializeVector(QVector<int> const &vector);

	trikControl::Brick &mBrick;
};

}
