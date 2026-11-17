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
#include <QtCore/QString>
#include <QtCore/QVariantList>

#include "classifierSensorInterface.h"
#include "deviceState.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class CameraManager;
class ClassifierSensorWorker;
class VideoPreviewSink;

class ClassifierSensor : public ClassifierSensorInterface
{
	Q_OBJECT

public:
	ClassifierSensor(const QString &port, const QSharedPointer<CameraManager> &cameraManager,
		trikHal::HardwareAbstractionInterface &hardware, const std::shared_ptr<VideoPreviewSink> &previewSink);
	~ClassifierSensor() override;

	Status status() const override;

	bool load(const QString &modelPath, const QString &labelsPath) override;
	void init(bool showOnDisplay) override;
	void stop(int flags) override;

	QString label() const override;
	int confidence() const override;
	int classIndex() const override;
	QVariantList top(int k) const override;

	void showLabels(int k) override;
	void hideLabels() override;

private:
	DeviceState mState;
	QScopedPointer<ClassifierSensorWorker> mWorker;
	const QString mPort;

	bool mLoaded = false;
	bool mStarted = false;
	bool mShowOnDisplay = false;
};

} // namespace trikControl
