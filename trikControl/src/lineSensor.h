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

#include <memory>

#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>

#include "deviceState.h"
#include "lineSensorInterface.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class CameraManager;
class LineSensorWorker;
class VideoPreviewSink;

/// CPU line sensor on top of LineSensorWorker.
///
/// The worker handles camera capture, RGA scaling, line detection and the
/// preview frame. This class only exposes the LineSensorInterface surface.
class LineSensor : public LineSensorInterface
{
	Q_OBJECT

public:
	LineSensor(const QString &port, const trikKernel::Configurer &configurer,
		const QSharedPointer<CameraManager> &cameraManager, trikHal::HardwareAbstractionInterface &hardware,
		const std::shared_ptr<VideoPreviewSink> &previewSink);
	~LineSensor() override;

	Status status() const override;

	QVector<int> read() override;
	QVector<int> getDetectParameters() const override;

public Q_SLOTS:
	void init(bool showOnDisplay) override;
	void detect() override;
	void stop(int flags = StopAll) override; // NOLINT(google-default-arguments)

private:
	DeviceState mState;
	QScopedPointer<LineSensorWorker> mWorker;
	const QString mPort;
};

} // namespace trikControl
