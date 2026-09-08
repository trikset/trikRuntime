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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <mutex>

#include <trikControl/videoSensorStopFlags.h>

#include "dspSensorBase.h"
#ifdef TRIK_DEBUG_FPS
#	include <trikKernel/fpsCounter.h>
#endif

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class OutputDeviceFileInterface;
}

namespace trikControl {

/// DSP-based JPEG encoder. Sits alongside Line/Object/Color sensors but is NOT
/// exposed through BrickInterface: it is an internal pipeline stage that
/// consumes camera frames, asks the DSP to encode them as JPEG, and streams the
/// encoded frames (with the mjpg-streamer frame delimiter) into a FIFO for
/// input_fifo.so to pick up.
///
/// onResult() is invoked from the DSP thread (the pipeline delivers Jpeg
/// results via a DirectConnection, so the encoded bytes in
/// trikDsp::OutArgs::jpegData are still valid - no copy). init()/stop() run on
/// the GUI thread; a small mutex serialises them against the FIFO write.
class JpegEncoderSensor : public QObject
{
	Q_OBJECT

public:
	/// Creates the encoder and resolves the per-port output FIFO from the config.
	JpegEncoderSensor(const QString &port, const trikKernel::Configurer &configurer,
		const trikHal::HardwareAbstractionInterface &hardwareAbstraction);
	~JpegEncoderSensor() override;

	/// Current device state.
	DeviceInterface::Status status() const;

	/// Receives an encoded frame from the DSP (DSP thread) and streams it into
	/// the FIFO.
	void onResult(const trikDsp::OutArgs &result);

Q_SIGNALS:
	void activateRequested(const trikDsp::InArgs &args, bool videoOut, bool canOpen);
	void stopRequested(int flags);
	void stopped();

public Q_SLOTS:
	/// Opens the FIFO and asks the DSP to start encoding frames.
	void init(uint8_t jpegQuality, bool ifBlackAndWhite, bool showOnDisplay = false);

	/// Closes the FIFO and asks the DSP to stop encoding.
	void stop(int flags = StopAll);

private:
	/// Writes one encoded frame plus the mjpg-streamer delimiter into the FIFO.
	/// Called with mFifoLock held.
	void writeFrame(const uint8_t *jpegData, uint32_t jpegSize);

	DspSensorHelper m;

	/// FIFO the encoded frames are streamed into (the write side of the
	/// mjpg-streamer input_fifo.so pipe).
	QScopedPointer<trikHal::OutputDeviceFileInterface> mFifoWriter;

	/// Frame delimiter used by mjpg-streamer's input_fifo.so to split frames.
	static const QByteArray sFrameDelimiter;

	/// Serialises onResult() (DSP thread) against init()/stop() (GUI thread).
	mutable std::mutex mFifoLock;

	/// True while frames are being dropped because the pipe is full. Lets the
	/// hot path log the transition instead of spamming one WARN per frame.
	bool mDropping = false;
	int mDroppedCount = 0;

#ifdef TRIK_DEBUG_FPS
	/// TEMP DEBUG: rate of JPEG frames written to the FIFO.
	trikKernel::FpsCounter mWriteFps {QStringLiteral("JpegEncoderSensor fifo")};
#endif
};

}
