/* Copyright 2026 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "jpegEncoderSensor.h"

#include <QsLog.h>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/outputDeviceFileInterface.h>

using namespace trikControl;

const QByteArray JpegEncoderSensor::sFrameDelimiter = QByteArrayLiteral("c3f97bee765fd86b209951ead9f8a583");

JpegEncoderSensor::JpegEncoderSensor(const QString &port, const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: m("Jpeg Encoder on " + port, configurer, port, trikDsp::Algorithm::Jpeg)
{
	// Each camera port streams through its own FIFO (mjpg-streamer's
	// input_fifo.so reads "/run/mjpg-encoder-<port>.out.fifo"); it may still be
	// overridden per port in the config ("outputFile").
	auto fifoName = QStringLiteral("/run/mjpg-encoder-%1.out.fifo").arg(port);
	fifoName = configurer.attributeByPort(port, "outputFile", &fifoName);

	mFifoWriter.reset(hardwareAbstraction.createOutputDeviceFile(fifoName));

	if (!m.state().isFailed()) {
		m.state().ready();
	}
}

JpegEncoderSensor::~JpegEncoderSensor()
{
	mFifoWriter->close();
	Q_EMIT stopped();
}

DeviceInterface::Status JpegEncoderSensor::status() const
{
	return m.state().status();
}

void JpegEncoderSensor::init(uint8_t jpegQuality, bool ifBlackAndWhite, bool showOnDisplay)
{
	if (!m.doInit(showOnDisplay)) {
		return;
	}

	{
		std::lock_guard<std::mutex> lock(mFifoLock);
		if (!mFifoWriter->open(trikHal::OutputDeviceFileInterface::OpenMode::NonBlockingBinary)) {
			QLOG_ERROR() << "JpegEncoderSensor: failed to open output fifo" << mFifoWriter->fileName();
			m.state().fail();
			return;
		}
	}

	m.inArgs().jpegQuality = jpegQuality;
	m.inArgs().ifBlackAndWhite = ifBlackAndWhite;

	Q_EMIT activateRequested(m.inArgs(), showOnDisplay, true);
}

void JpegEncoderSensor::stop(int flags)
{
	m.doStop();
	{
		std::lock_guard<std::mutex> lock(mFifoLock);
		mFifoWriter->close();
	}
	Q_EMIT stopRequested(flags);
	Q_EMIT stopped();
}

void JpegEncoderSensor::onResult(const trikDsp::OutArgs &result)
{
	// Runs on the DSP thread (DirectConnection), right after processFrame():
	// out.jpegData points into the DSP output buffer and is only valid until
	// the next step, so it must be written out synchronously here.
	if (!result.jpegData || result.jpegSize == 0) {
		return;
	}

	std::lock_guard<std::mutex> lock(mFifoLock);
	writeFrame(result.jpegData, result.jpegSize);
#ifdef TRIK_DEBUG_FPS
	mWriteFps.tick();
#endif
}

void JpegEncoderSensor::writeFrame(const uint8_t *jpegData, uint32_t jpegSize)
{
	const QByteArray frame(reinterpret_cast<const char *>(jpegData), static_cast<int>(jpegSize));

	// Non-blocking: drops the whole frame when the pipe is full (slow consumer),
	// so a truncated frame (missing its delimiter) never corrupts the stream.
	if (mFifoWriter->write(frame + sFrameDelimiter)) {
		if (mDropping) {
			QLOG_INFO() << "JpegEncoderSensor: pipe drained," << mDroppedCount << "frame(s) were dropped";
			mDropping = false;
			mDroppedCount = 0;
		}
		return;
	}

	// Log once when dropping starts and once (with the count) when it stops,
	// instead of a WARN per dropped frame (~20/s) in this hot path.
	if (!mDropping) {
		QLOG_WARN() << "JpegEncoderSensor: dropped frame, pipe full (further drops suppressed)";
		mDropping = true;
	}
	++mDroppedCount;
}
