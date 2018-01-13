#pragma once

#include <QtCore/QScopedPointer>
#include <QtMultimedia/QCamera>

#include "cameraImplementationInterface.h"
#include "declSpec.h"

namespace trikControl {

class TRIKCONTROL_EXPORT QtCameraImplementation : public CameraImplementationInterface
{
public:

	QtCameraImplementation(const QString & port);

	QVector<uint8_t> getPhoto() override;

    ~QtCameraImplementation() = default;

private:
	QScopedPointer<QCamera> mCamera;
};

}
