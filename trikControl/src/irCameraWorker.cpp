#include "irCameraWorker.h"

#include <QsLog.h>
#include <QTimer>

namespace trikControl {

IrCameraWorkerMLX90640::IrCameraWorkerMLX90640(uint8_t i2cAddr): mI2cAddr(i2cAddr), mState("IR Camera worker")
{
	mFrame.resize(IMAGE_SIZE);
	mFrameBuffer.resize(FRAME_SIZE);
}

IrCameraWorkerMLX90640::~IrCameraWorkerMLX90640()
{
}

void IrCameraWorkerMLX90640::init()
{
	if (mState.isReady()) {
		QLOG_ERROR() << "Trying to init Ir Camera that is already running, ignoring";
		return;
	}
	if (mState.status() == DeviceInterface::Status::starting) {
		// Sensor is already starting.
		QLOG_ERROR() << "Trying to start Ir Camera that is already starting, ignoring";
		return;
	}
	mState.start();

	// Library written primary in C style, so no exceptions, only error codes
	int status;

	status = MLX90640_SetRefreshRate(mI2cAddr, mRefreshRate);
	if (status != 0) {
		QString info = status == -1 ? "Got NACK." : "Written incorrect value";
		QLOG_ERROR() << "Failed to set IR camera refresh rate. " << info;
		mState.fail();
	}

	// Ensure that camera works in recommended mode
	status = MLX90640_SetChessMode(mI2cAddr);
	if (status != 0) {
		QString info = status == -1 ? "Got NACK." : "Written incorrect value";
		QLOG_ERROR() << "Failed to set IR camera mode to chess. " << info;
		mState.fail();
	}

	QVector<uint16_t> eeprom(EEPROM_SIZE);
	status = MLX90640_DumpEE(mI2cAddr, eeprom.data());
	if (status == -1) {
		QLOG_ERROR() << "Failed to dump IR camera EEPROM. Got NACK.";
		mState.fail();
	}

	status = MLX90640_ExtractParameters(eeprom.data(), &mParams);
	if (status == -7) {
		QLOG_ERROR() << "Failed extract parameters from IR camerea EEPROM.";
		mState.fail();
	}

	mState.ready();
	QLOG_INFO() << "Initialized Ir Camera worker";

	QTimer::singleShot(0, this, &IrCameraWorkerMLX90640::readBuffer);
}

void IrCameraWorkerMLX90640::stop()
{
	if (mState.isReady()) {
		mState.stop();
		emit stopped();
		mState.off();
	}
}

void IrCameraWorkerMLX90640::readBuffer()
{
	if (!mState.isReady()) {
		return;
	}

	int status = MLX90640_GetFrameData(mI2cAddr, mFrameBuffer.data());
	if (status < 0) {
		QString info = status == -1 ? "Got NACK." : "Frame is corrupted or can't acquire it.";
		QLOG_ERROR() << "Failed to get frame from IR. " << info;
		return;
	}

	std::transform(mFrameBuffer.cbegin(), mFrameBuffer.cend() - (FRAME_SIZE - IMAGE_SIZE)
				, std::begin(mParams.offset)
				, mFrame.begin()
				, [](uint16_t pixel, uint16_t offset) {
						auto col = (((static_cast<int16_t>(pixel) - static_cast<int16_t>(offset) + 256)) >> 1) & 0xFF;
						return static_cast<uint8_t>(col);
				});

	QTimer::singleShot(0, this, &IrCameraWorkerMLX90640::readBuffer);
	emit newFrame(mFrame);
}

}
