#include <QImage>
#include "v4l2CameraImplementation.h"
#include "QsLog.h"
using namespace trikControl;

V4l2CameraImplementation::V4l2CameraImplementation(const QString &port, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mHal(hardwareAbstraction)
	, mPort(port)
{
}

QVector<uint8_t> V4l2CameraImplementation::getPhoto()
{
	auto result = mHal.captureV4l2StillImage(mPort, getTempDir());
	if (result.empty())
		return result;

	auto image = QImage(result.data(), 320, 240, QImage::Format_RGB888);
	auto saved = image.save(getTempDir()+"/photo.jpg", "JPG");
	if (!saved) {
		QLOG_WARN() << "Failed to save captured image";
	}
	return result;
}
