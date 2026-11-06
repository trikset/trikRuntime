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
#include <cstdint>
#include <memory>

#include <QtCore/QMutex>
#include <QtCore/QVector>

#include "colorUtils.h"
#include "cpuAlgorithmWorker.h"

namespace trikControl {

/// CPU line sensor worker.
///
/// The base class handles capture, RGA scaling, preview buffers and publishing.
/// This class detects the line within the configured V range, computes its
/// position, crossroads probability and mass, and draws the preview overlay
/// directly into the frame.
class LineSensorWorker : public CpuAlgorithmWorker
{
public:
	LineSensorWorker(QSharedPointer<CameraManager> camera, QString port, trikHal::HardwareAbstractionInterface &hal,
		std::shared_ptr<VideoPreviewSink> previewSink, int previewBufferCount = 4, QObject *parent = nullptr);

	/// Requests a one-shot HSV range detection on the next processed frame.
	void detect();

	/// Returns {hue, saturation, value, hueTolerance, saturationTolerance, valueTolerance}
	/// from the last detect() call.
	QVector<int> detectParameters() const;

	/// Returns {x, crossroadsProbability, mass} from the last processed frame.
	QVector<int> reading() const;

	/// Sets the tolerance multiplier applied to the auto-detected V range.
	void setToleranceFactor(qreal factor)
	{
		mToleranceFactor = factor;
	}

protected:
	bool modifiesOutputFrame() const override
	{
		return true;
	}
	bool processFrame(const BgraFrameView &frame) override;

	/// All visual changes are applied directly to the frame in processFrame();
	/// this sensor does not build QPainterPath overlays.
	OverlayResult buildOverlay(const BgraFrameView &frame) override;

private:
	/// Inclusive range of one HSV component (0..255).
	struct HsvRange {
		uint8_t from = 0;
		uint8_t to = 0;
	};

	/// Whether @p hsv falls into the current detection window.
	bool matches(const Hsv8 &hsv) const;

	/// Runs one-shot range detection on @p frame and updates the detection window.
	void detectRange(const BgraFrameView &frame);

	/// Raw detection result exposed through LineSensor::getDetectParameters().
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
	DetectParameters mDetectParameters;
	mutable QMutex mDetectParametersLock;

	/// Detection window, carried over from the previous line sensor implementation.
	HsvRange mHueRange {0, 255};
	HsvRange mSaturationRange {0, 255};
	HsvRange mValueRange {0, 0};

	/// True when the hue range wraps through 0/255 (for example 240..30).
	/// Hue is circular, S and V are not.
	bool mHueWraps = false;

	/// Latest line detection result exposed through LineSensor::read().
	struct LineReading {
		int x = 0; ///< Line center X in [-100, 100], relative to frame center.
		int cross = 0; ///< Crossroads probability in percent.
		int mass = 0; ///< Fraction of frame occupied by the line in percent.
	};

	LineReading mReading;
	mutable QMutex mReadingLock;
};

} // namespace trikControl
