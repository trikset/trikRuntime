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

#include "classifierSensor.h"

#include <QsLog.h>

#include <trikHal/hardwareAbstractionInterface.h>

#include "algorithm/classifierSensorWorker.h"
#include "cameraManager.h"

using namespace trikControl;

ClassifierSensor::ClassifierSensor(const QString &port, const QSharedPointer<CameraManager> &cameraManager,
	trikHal::HardwareAbstractionInterface &hardware, const std::shared_ptr<VideoPreviewSink> &previewSink)
	: mState("Classifier Sensor on " + port)
	, mPort(port)
{
	const auto portId = cameraManager ? cameraManager->portId(port) : -1;
	if (portId < 0) {
		QLOG_ERROR() << "ClassifierSensor: port" << port << "is not a configured videoDevice";
		mState.fail();
		return;
	}

	mWorker.reset(new ClassifierSensorWorker(cameraManager, port, hardware, previewSink));
	mState.ready();
}

ClassifierSensor::~ClassifierSensor()
{
	if (mWorker) {
		mWorker->stop(StopAll);
	}
}

ClassifierSensor::Status ClassifierSensor::status() const
{
	return mState.status();
}

bool ClassifierSensor::load(const QString &modelPath, const QString &labelsPath)
{
	if (mState.isFailed() || !mWorker) {
		return false;
	}

	const auto wasStarted = mStarted;
	if (wasStarted) {
		mWorker->stop(StopAll);
		mStarted = false;
	}

	auto error = QString {};
	if (!mWorker->loadClassifier(modelPath, labelsPath, &error)) {
		QLOG_ERROR() << "ClassifierSensor: failed to load model on" << mPort << ":" << error;
		mLoaded = false;
		return false;
	}

	mLoaded = true;

	if (wasStarted) {
		if (!mState.isReady()) {
			mState.ready();
		}
		mWorker->start(mShowOnDisplay);
		mStarted = true;
	}

	return true;
}

void ClassifierSensor::init(bool showOnDisplay)
{
	if (mState.isFailed() || !mWorker || mWorker->isRunning()) {
		return;
	}

	if (!mLoaded) {
		QLOG_ERROR() << "ClassifierSensor: no model loaded on" << mPort;
		return;
	}

	mShowOnDisplay = showOnDisplay;

	// The sensor is ready right after construction; a re-init after stop()
	// brings it back from off.
	if (!mState.isReady()) {
		mState.ready();
	}

	mWorker->start(showOnDisplay);
	mStarted = true;
	QLOG_INFO() << "ClassifierSensor: started on" << mPort;
}

// NOLINTNEXTLINE(google-default-arguments)
void ClassifierSensor::stop(int flags)
{
	const auto releaseCamera = (flags & (StopStream | StopAll)) != 0;

	if (mWorker) {
		mWorker->stop(flags);
	}
	mStarted = false;

	if (releaseCamera && mState.isReady()) {
		mState.stop();
		mState.off();
	}

	Q_EMIT stopped();
}

QString ClassifierSensor::label() const
{
	return mWorker ? mWorker->label() : QString();
}

int ClassifierSensor::confidence() const
{
	return mWorker ? mWorker->confidence() : 0;
}

int ClassifierSensor::classIndex() const
{
	return mWorker ? mWorker->classIndex() : -1;
}

QVariantList ClassifierSensor::top(int k) const
{
	return mWorker ? mWorker->top(k) : QVariantList {};
}

void ClassifierSensor::showLabels(int k)
{
	if (mWorker) {
		mWorker->showLabels(k);
	}
}

void ClassifierSensor::hideLabels()
{
	if (mWorker) {
		mWorker->hideLabels();
	}
}
