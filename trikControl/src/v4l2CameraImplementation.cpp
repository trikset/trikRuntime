#include "v4l2CameraImplementation.h"

using namespace trikControl;

V4l2CameraImplementation::V4l2CameraImplementation(const QString &port, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mHal(hardwareAbstraction)
	, mPort(port)
{
}

QVector<uint8_t> V4l2CameraImplementation::getPhoto()
{
	return mHal.captureV4l2StillImage(mPort, getTempDir());
}
