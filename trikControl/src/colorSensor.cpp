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

#include "colorSensor.h"

#include <QtCore/QVariant>

#include <QsLog.h>

#include <trikKernel/configurer.h>

#include <trikHal/hardwareAbstractionInterface.h>

#include "cameraManager.h"
#include "colorSensorWorker.h"
#include "configurerHelper.h"

using namespace trikControl;

ColorSensor::ColorSensor(const QString &port, const trikKernel::Configurer &configurer,
	const QSharedPointer<CameraManager> &cameraManager, trikHal::HardwareAbstractionInterface &hardware,
	const std::shared_ptr<VideoPreviewSink> &previewSink)
	: mState("Color Sensor on " + port)
	, mPort(port)
{
	mM = ConfigurerHelper::configureChildInt(configurer, mState, port, "colorSensor", "m");
	mN = ConfigurerHelper::configureChildInt(configurer, mState, port, "colorSensor", "n");

	if (mM <= 0 || mM > maxColorSensorGridSize || mN <= 0 || mN > maxColorSensorGridSize || mState.isFailed()) {
		mState.fail();
		return;
	}

	const auto portId = cameraManager ? cameraManager->portId(port) : -1;
	if (portId < 0) {
		QLOG_ERROR() << "ColorSensor: port" << port << "is not a configured videoDevice";
		mState.fail();
		return;
	}

	mWorker.reset(new ColorSensorWorker(cameraManager, port, mM, mN, hardware, previewSink));
	mState.ready();
}

ColorSensor::~ColorSensor()
{
	if (mWorker) {
		mWorker->stop(StopAll);
	}
}

ColorSensor::Status ColorSensor::status() const
{
	return mState.status();
}

void ColorSensor::init(bool showOnDisplay)
{
	if (mState.isFailed() || !mWorker || mWorker->isRunning()) {
		return;
	}

	// The sensor is ready right after construction; a re-init after stop()
	// brings it back from off.
	if (!mState.isReady()) {
		mState.ready();
	}

	mWorker->start(showOnDisplay);
	QLOG_INFO() << "ColorSensor: started on" << mPort << "grid" << mM << 'x' << mN;
}

QVector<int> ColorSensor::read(int m, int n)
{
	if (m <= 0 || n <= 0 || m > mM || n > mN) {
		return {-1, -1, -1};
	}

	if (!mWorker || !mWorker->isRunning()) {
		return {0, 0, 0};
	}

	return mWorker->cell(m, n);
}

// NOLINTNEXTLINE(google-default-arguments)
void ColorSensor::stop(int flags)
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
