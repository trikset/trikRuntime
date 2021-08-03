#include "src/vectorSensorWorkerIIO.h"

trikControl::VectorSensorWorkerIIO::VectorSensorWorkerIIO(const QString &iioDevFile, trikControl::DeviceState &state
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mIIOFile(hardwareAbstraction.createIIOFile(iioDevFile))
	, mState(state)
{

}

QVector<int> trikControl::VectorSensorWorkerIIO::read()
{
	return mReading;
}

void trikControl::VectorSensorWorkerIIO::deinitialize()
{

}

void trikControl::VectorSensorWorkerIIO::init()
{

}
