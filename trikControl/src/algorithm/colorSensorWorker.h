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

#include <array>
#include <memory>

#include <QtCore/QMutex>
#include <QtCore/QVector>

#include <trikControl/colorSensorInterface.h>

#include "cpuAlgorithmWorker.h"

namespace trikControl {

/// CPU implementation of the MxN color grid on top of CpuAlgorithmWorker.
///
/// The base class handles capture, RGA scaling, preview buffers and publishing.
/// This class only computes the grid and the overlay.
class ColorSensorWorker : public CpuAlgorithmWorker
{
public:
	ColorSensorWorker(QSharedPointer<CameraManager> camera, QString port, int m, int n,
		trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);

	/// Last RGB value of cell (@p mIndex, @p nIndex), 1-based.
	QVector<int> cell(int mIndex, int nIndex) const;

protected:
	bool processFrame(const BgraFrameView &frame) override;
	OverlayResult buildOverlay(const BgraFrameView &frame) override;

private:
	/// Fixed-capacity grid of RGB triplets, up to maxColorSensorGridSize.
	using Color = std::array<int, 3>;
	using Grid = std::array<std::array<Color, maxColorSensorGridSize>, maxColorSensorGridSize>;

	/// Dirty adaptation of the existing algorithm from the previous version
	/// (semantically equivalent). It takes ~2-3 ms to process each frame (320x240) on the
	/// current platform (ARMv8.2), so it does not need optimization for now.
	Grid buildGridFromBgra(const BgraFrameView &frame) const;
	PreviewOverlay makeOverlay(const Grid &grid, int width, int height) const;

	const int mM;
	const int mN;

	mutable QMutex mGridLock;
	Grid mGrid {};
	Grid mLastOverlayGrid {};
	bool mOverlayGridValid = false;
	PreviewOverlay mLastOverlay;
	quint32 mOverlayGeneration = 0;
};

} // namespace trikControl
