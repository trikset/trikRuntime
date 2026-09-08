/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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
#include <QtCore/QVector>
#include <QMutex>

#include "cameraDeviceInterface.h"
#include "cameraImplementationInterface.h"
#include <trikControl/trikControlDeclSpec.h>

class QObject;
class QThread;

namespace trikKernel {
class Configurer;
}
namespace trikControl {
class CameraManager;
}

namespace trikControl {

class CameraDevice : public CameraDeviceInterface
{
public:
	CameraDevice(const QString &port, const QString &mediaPath, const trikKernel::Configurer &configurer,
		const QSharedPointer<CameraManager> &cameraManager);

	QVector<uint8_t> getPhoto() override;
	Status status() const override;
	~CameraDevice() override;

private:
	QMutex mCameraMutex;
	QScopedPointer<CameraImplementationInterface> mCameraImpl;
	/// Persistent worker thread the photo capture runs on. Created once in the
	/// constructor and reused for every getPhoto(), so a thread is not spawned
	/// per photo.
	QScopedPointer<QThread> mCameraThread;
	/// Context QObject living on mCameraThread, used as the target for queued
	/// functor invocations (a plain QObject is enough).
	QScopedPointer<QObject> mCameraWorker;
};

}
