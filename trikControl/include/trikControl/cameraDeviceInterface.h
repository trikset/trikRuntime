#pragma once

#include <QtCore/QVector>

#include "declSpec.h"
#include "deviceInterface.h"

namespace trikControl {

class TRIKCONTROL_EXPORT CameraDeviceInterface : public DeviceInterface
{
public:

    virtual QVector<uint8_t> getPhoto() = 0;

    virtual Status status() const = 0;
};

}
