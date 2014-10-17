#pragma once

#include <trikKernel/connection.h>
#include <trikControl/brick.h>

namespace trikTelemetry {

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
