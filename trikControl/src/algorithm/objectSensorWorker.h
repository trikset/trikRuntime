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
#include "objectDetector.h"

namespace trikControl {

/// CPU object sensor worker.
///
/// The base class handles capture, RGA scaling, preview buffers and publishing.
/// This class owns the ObjectDetector and exposes its reading to the sensor.
class ObjectSensorWorker : public CpuAlgorithmWorker
{
public:
	ObjectSensorWorker(QSharedPointer<CameraManager> camera, QString port,
		trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);

	/// Requests a one-shot HSV range detection on the next processed frame.
	void detect();

	/// Returns {x, y, size} of the largest target from the last processed frame.
	QVector<int> reading() const;

	/// Returns {hue, saturation, value, hueTolerance, saturationTolerance, valueTolerance}
	/// from the last detect() call.
	QVector<int> detectParameters() const;

	/// Sets the tolerance multiplier applied to the auto-detected HSV range.
	void setToleranceFactor(qreal factor);

protected:
	/// The algorithm draws the preview directly into the frame buffer.
	bool modifiesOutputFrame() const override
	{
		return true;
	}

	/// Runs the detector and draws the preview overlay into the frame.
	bool processFrame(const BgraFrameView &frame) override;

	/// All visual changes are applied directly to the frame in processFrame();
	/// this sensor does not build QPainterPath overlays.
	OverlayResult buildOverlay(const BgraFrameView &frame) override;

private:
	/// Draws the magenta guide lines that mark the detection zone boundaries.
	void drawTargetGuides(const BgraFrameView &frame) const;

	/// Raw detection parameters exposed through the sensor interface.
	struct DetectParameters {
		int hue = 0; ///< Dominant hue in [0, 359].
		int saturation = 0; ///< Dominant saturation in [0, 100].
		int value = 0; ///< Dominant value in [0, 100].
		int hueTolerance = 0; ///< Hue tolerance in [0, 359].
		int saturationTolerance = 0; ///< Saturation tolerance in [0, 100].
		int valueTolerance = 0; ///< Value tolerance in [0, 100].
	};

	ObjectDetector mDetector; ///< Frame-to-target detector.
	ObjectDetector::Result mResult; ///< Latest detection result.
	mutable QMutex mResultLock; ///< Guards mResult.

	std::atomic<bool> mDetectRequested {false}; ///< A one-shot detect() is pending.
	qreal mToleranceFactor = 1.0; ///< Multiplier for the detected range.

	DetectParameters mDetectParameters; ///< Latest raw detection parameters.
	mutable QMutex mDetectParametersLock; ///< Guards mDetectParameters.
};

} // namespace trikControl
