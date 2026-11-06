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

#include <atomic>

#include <QtCore/QMutex>
#include <QtCore/QVector>

#include "cpuAlgorithmWorker.h"

namespace trikControl {

/// CPU object sensor worker.
///
/// The base class handles capture, RGA scaling, preview buffers and publishing.
/// This class is intended to detect a coloured object, track its position and
/// size and auto-detect the HSV range on detect().
class ObjectSensorWorker : public CpuAlgorithmWorker
{
public:
	ObjectSensorWorker(QSharedPointer<CameraManager> camera, QString port,
		trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);

	/// Requests a one-shot HSV range detection on the next processed frame.
	void detect();

	/// Returns {x, y, size} from the last processed frame.
	QVector<int> reading() const;

	/// Returns {hue, saturation, value, hueTolerance, saturationTolerance, valueTolerance}
	/// from the last detect() call.
	QVector<int> detectParameters() const;

	/// Sets the tolerance multiplier applied to the auto-detected HSV range.
	void setToleranceFactor(qreal factor);

protected:
	bool processFrame(const BgraFrameView &frame) override;
	OverlayResult buildOverlay(const BgraFrameView &frame) override;

private:
	struct Reading {
		int x = 0;
		int y = 0;
		int size = 0;
	};

	struct DetectParameters {
		int hue = 0;
		int saturation = 0;
		int value = 0;
		int hueTolerance = 0;
		int saturationTolerance = 0;
		int valueTolerance = 0;
	};

	std::atomic<bool> mDetectRequested {false};
	qreal mToleranceFactor = 1.0;

	Reading mReading;
	mutable QMutex mReadingLock;

	DetectParameters mDetectParameters;
	mutable QMutex mDetectParametersLock;
};

} // namespace trikControl
