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

#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>

#include "colorSensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class CameraManager;
class ColorAlgorithmWorker;
class VideoPreviewSink;

/// CPU color sensor: an MxN grid of average RGB colors read from a camera.
///
/// The sensor owns a ColorAlgorithmWorker pipeline that pulls the newest frame
/// from CameraManager, scales it with the RGA and computes the HSV grid. init()
/// starts the pipeline; stop() stops it and releases the camera.
class ColorSensor : public ColorSensorInterface
{
	Q_OBJECT

public:
	ColorSensor(const QString &port, const trikKernel::Configurer &configurer,
		const QSharedPointer<CameraManager> &cameraManager,
		trikHal::HardwareAbstractionInterface &hardware, VideoPreviewSink *previewSink);
	~ColorSensor() override;

	Status status() const override;

	QVector<int> read(int m, int n) override;

public Q_SLOTS:
	void init(bool showOnDisplay) override;
	void stop(int flags = StopAll) override; // NOLINT(google-default-arguments)

private:
	DeviceState mState;
	QScopedPointer<ColorAlgorithmWorker> mWorker;
	QString mPort;
	int mM = 0;
	int mN = 0;
};

} // namespace trikControl
