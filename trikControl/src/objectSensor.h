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
#include "objectSensorInterface.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class CameraManager;
class ObjectSensorWorker;
class VideoPreviewSink;

/// CPU object sensor on top of ObjectSensorWorker.
///
/// The worker handles camera capture, RGA scaling, color detection and the
/// preview overlay. This class exposes the ObjectSensorInterface surface and
/// forwards the configured tolerance factor to the worker.
class ObjectSensor : public ObjectSensorInterface
{
	Q_OBJECT

public:
	ObjectSensor(const QString &port, const trikKernel::Configurer &configurer,
		const QSharedPointer<CameraManager> &cameraManager, trikHal::HardwareAbstractionInterface &hardware,
		const std::shared_ptr<VideoPreviewSink> &previewSink);
	~ObjectSensor() override;

	Status status() const override;

	QVector<int> read() override;
	QVector<int> getDetectParameters() const override;

public Q_SLOTS:
	void init(bool showOnDisplay) override;
	void detect() override;
	void stop(int flags = StopAll) override; // NOLINT(google-default-arguments)

private:
	DeviceState mState;
	QScopedPointer<ObjectSensorWorker> mWorker;
	const QString mPort;
};

} // namespace trikControl
