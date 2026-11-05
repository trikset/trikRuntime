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

#include <QtCore/QMutex>
#include <QtCore/QVector>

#include "cpuAlgorithmWorker.h"

namespace trikControl {

/// CPU implementation of the MxN color grid on top of CpuAlgorithmWorker.
///
/// The base class handles capture, RGA scaling, preview buffers and publishing.
/// This class only computes the grid and the overlay.
class ColorAlgorithmWorker : public CpuAlgorithmWorker
{
public:
	ColorAlgorithmWorker(QSharedPointer<CameraManager> camera, int portId, QString port, int m, int n,
		trikHal::HardwareAbstractionInterface &hal, VideoPreviewSink *previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);

	/// Last RGB value of cell (@p mIndex, @p nIndex), 1-based.
	QVector<int> cell(int mIndex, int nIndex) const;

protected:
	trikHal::FrameFormat scaledFormat(int srcWidth, int srcHeight) const override;
	bool processFrame(const BgraFrameView &frame) override;
	OverlayResult buildOverlay(const BgraFrameView &frame) override;

private:
	/// Fixed 3x3 grid of RGB triplets (the DSP supports at most 3x3 cells).
	struct Grid {
		int colors[3][3][3] = {};
	};

	Grid buildGridFromBgra(const BgraFrameView &frame) const;
	PreviewOverlay makeOverlay(const Grid &grid, int width, int height) const;

	int mM;
	int mN;

	mutable QMutex mGridLock;
	Grid mGrid;
	Grid mLastOverlayGrid;
	bool mOverlayGridValid = false;
	PreviewOverlay mLastOverlay;
	quint32 mOverlayGeneration = 0;
};

} // namespace trikControl
