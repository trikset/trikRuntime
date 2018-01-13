#pragma once

#include "cameraImplementationInterface.h"
#include <trikHal/hardwareAbstractionInterface.h>
#include "declSpec.h"


namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class V4l2CameraImplementation : public CameraImplementationInterface
{
public:
	V4l2CameraImplementation(const QString &port, trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	QVector<uint8_t> getPhoto();

	~V4l2CameraImplementation() = default;
private:
	trikHal::HardwareAbstractionInterface & mHal;
	QString mPort;
};

}
