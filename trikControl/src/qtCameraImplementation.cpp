#include <QtCore/QScopedPointer>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QCameraImageCapture>
#include <QtMultimedia/QCameraInfo>

#include "qtCameraImplementation.h"
#include <QsLog.h>

using namespace trikControl;

QtCameraImplementation::QtCameraImplementation(const QString & port)
{
	QLOG_INFO() << "Available cameras:" << QCameraInfo::availableCameras().count();
	for (auto & cameraInfo : QCameraInfo::availableCameras()) {
		if (cameraInfo.deviceName() == port) {
				decltype(mCamera) tmp(new QCamera(cameraInfo));
				tmp.swap(mCamera);
				break;
		}
	}

	if (!mCamera) {
		QLOG_ERROR() << "Failed to initialize camera for " << port << " from available cameras" << QCameraInfo::availableCameras();
	}
}

QVector<uint8_t> QtCameraImplementation::getPhoto()
{
	if(!mCamera)
		return QVector<uint8_t>();

	QScopedPointer<QCameraImageCapture> imageCapture (new QCameraImageCapture(mCamera.data()));

	imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

	const auto & formats = imageCapture->supportedBufferFormats();
	QLOG_INFO() << "Supported buffer formats: " << formats;

	auto camera = mCamera.data();
	QObject::connect(imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged, [this, &imageCapture, camera](bool ready)
		{
			if (ready) {
				camera->searchAndLock();
				imageCapture->capture(getTempDir() + "/photo.jpg");
				camera->unlock();
			}
		}
	);

	QVector<uint8_t> imageByteVector;

	QObject::connect(imageCapture.data(), &QCameraImageCapture::imageCaptured, [&imageByteVector] (int, const QImage &imgOrig)
		{
			// Some possible formats:
			// QImage::Format_RGB32
			// QImage::Format_RGB888
			// QImage::Format_RGB16
			// QImage::Format_Grayscale8
			// QImage::Format_Mono
			constexpr auto DESIRED_FORMAT = QImage::Format_RGB888;

			constexpr auto SIZE_X = 320;
			constexpr auto SIZE_Y = 240;

			const QImage &img = imgOrig.format() == DESIRED_FORMAT ? imgOrig : imgOrig.convertToFormat(DESIRED_FORMAT);
			const QImage &scaledImg = img.height() == SIZE_X && img.width() == SIZE_Y ? img : img.scaled(SIZE_X, SIZE_Y); //, Qt::KeepAspectRatioByExpanding
			auto cb = scaledImg.constBits();
			imageByteVector.resize(scaledImg.byteCount());
			std::copy(cb, cb + scaledImg.byteCount(), imageByteVector.begin());
		}
	);

	mCamera->setCaptureMode(QCamera::CaptureStillImage);
	mCamera->start();
	QEventLoop eventLoop;
	QTimer watchdog;
	watchdog.setInterval(1000);
	watchdog.setSingleShot(true);
	QObject::connect(&watchdog, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
	QObject::connect(imageCapture.data(), &QCameraImageCapture::imageAvailable, [&eventLoop, &imageByteVector](int, const QVideoFrame &) {
				eventLoop.quit();
			}
		);
	eventLoop.exec();
	watchdog.stop();

	return imageByteVector;
}
