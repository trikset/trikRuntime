#pragma once

#include <QtCore/QScopedPointer>
#include <QtCore/QVector>

#include "cameraDeviceInterface.h"
#include "cameraImplementationInterface.h"
#include "declSpec.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class TRIKCONTROL_EXPORT CameraDevice : public CameraDeviceInterface
{
public:
	CameraDevice( const QString & mMediaPath
				 , trikKernel::Configurer const &configurer
				 , trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	QVector<uint8_t> getPhoto() override;

	Status status() const override;

	~CameraDevice() = default;

private:
	QScopedPointer<CameraImplementationInterface> mCameraImpl;
};

}
