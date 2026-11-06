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

#include "lineSensor.h"

#include <QsLog.h>

#include <trikKernel/configurer.h>

#include <trikHal/hardwareAbstractionInterface.h>

#include "cameraManager.h"
#include "algorithm/lineSensorWorker.h"

using namespace trikControl;

LineSensor::LineSensor(const QString &port, const trikKernel::Configurer &configurer,
	const QSharedPointer<CameraManager> &cameraManager, trikHal::HardwareAbstractionInterface &hardware,
	const std::shared_ptr<VideoPreviewSink> &previewSink)
	: mState("Line Sensor on " + port)
	, mPort(port)
{
	QString defaultTolerance = QStringLiteral("1.0");
	bool toleranceOk = false;
	const auto tolerance = configurer.childAttributeByPort(port, "lineSensor", "toleranceFactor", &defaultTolerance)
	                               .toDouble(&toleranceOk);
	if (!toleranceOk) {
		QLOG_ERROR() << "LineSensor: invalid toleranceFactor on" << port;
		mState.fail();
		return;
	}

	const auto portId = cameraManager ? cameraManager->portId(port) : -1;
	if (portId < 0) {
		QLOG_ERROR() << "LineSensor: port" << port << "is not a configured videoDevice";
		mState.fail();
		return;
	}

	mWorker.reset(new LineSensorWorker(cameraManager, port, hardware, previewSink));
	mWorker->setToleranceFactor(tolerance);
	mState.ready();
}

LineSensor::~LineSensor()
{
	if (mWorker) {
		mWorker->stop(StopAll);
	}
}

LineSensor::Status LineSensor::status() const
{
	return mState.status();
}

void LineSensor::init(bool showOnDisplay)
{
	if (mState.isFailed() || !mWorker || mWorker->isRunning()) {
		return;
	}

	if (!mState.isReady()) {
		mState.ready();
	}

	mWorker->start(showOnDisplay);
	QLOG_INFO() << "LineSensor: started on" << mPort;
}

void LineSensor::detect()
{
	if (mState.isReady() && mWorker) {
		mWorker->detect();
	}
}

QVector<int> LineSensor::read()
{
	return mWorker ? mWorker->reading() : QVector<int> {0, 0, 0};
}

QVector<int> LineSensor::getDetectParameters() const
{
	return mWorker ? mWorker->detectParameters() : QVector<int> {0, 0, 0, 0, 0, 0};
}

// NOLINTNEXTLINE(google-default-arguments)
void LineSensor::stop(int flags)
{
	const auto releaseCamera = (flags & (StopStream | StopAll)) != 0;

	if (mWorker) {
		mWorker->stop(flags);
	}

	if (releaseCamera && mState.isReady()) {
		mState.stop();
		mState.off();
	}

	Q_EMIT stopped();
}
