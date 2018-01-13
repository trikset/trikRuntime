#include "cameraDevice.h"
#include "qtCameraImplementation.h"
#include "v4l2CameraImplementation.h"

#include "QsLog.h"
#include <trikKernel/configurer.h>

namespace trikControl {

CameraDevice::CameraDevice(const QString & mediaPath, trikKernel::Configurer const &configurer
							, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
{
	Q_UNUSED(hardwareAbstraction);

	QString type = configurer.attributeByDevice("camera", "type");
	QString src = configurer.attributeByDevice("camera", "src");
	if (type == "qtmultimedia") {
		decltype(mCameraImpl)(new QtCameraImplementation(src)).swap(mCameraImpl);
	} else if (type == "v4l2") {
				decltype(mCameraImpl)(new V4l2CameraImplementation(src, hardwareAbstraction)).swap(mCameraImpl);
	} else {
		QLOG_ERROR() << "Unknown camera device type: " << type;
	}
	if (mCameraImpl) {
		mCameraImpl->setTempDir(mediaPath);
	} else {
		QLOG_ERROR() << "Failed to initialize camera device for " << src;
	}
}

QVector<uint8_t> CameraDevice::getPhoto() {
	if (mCameraImpl)
		return mCameraImpl->getPhoto();
	else
		return QVector<uint8_t>();
}


CameraDevice::Status CameraDevice::status() const {
    return CameraDevice::Status::ready;
}
}
