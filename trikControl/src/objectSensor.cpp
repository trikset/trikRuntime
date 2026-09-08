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

#include "objectSensor.h"

#include <trikKernel/configurer.h>

#include "configurerHelper.h"

using namespace trikControl;

ObjectSensor::ObjectSensor(const QString &port, const trikKernel::Configurer &configurer)
	: m("Object Sensor on " + port, configurer, port, trikDsp::Algorithm::Object)
{
	mToleranceFactor = ConfigurerHelper::configureChildReal(
	                       configurer, m.state(), port, "objectSensor", "toleranceFactor");

	if (!m.state().isFailed()) {
		m.state().ready();
	}
}

ObjectSensor::~ObjectSensor()
{
	Q_EMIT stopped();
}

ObjectSensor::Status ObjectSensor::status() const
{
	return m.state().status();
}

void ObjectSensor::init(bool showOnDisplay)
{
	if (!m.doInit(showOnDisplay))
		return;

	Q_EMIT activateRequested(m.inArgs(), showOnDisplay, true);
}

void ObjectSensor::detect()
{
	if (status() == Status::ready) {
		m.inArgs().autoDetect = true;
		Q_EMIT activateRequested(m.inArgs(), m.videoOut(), false);
	}
}

QVector<int> ObjectSensor::read()
{
	QReadLocker locker(&mReadingLock);
	return mReading;
}

void ObjectSensor::stop(int flags) // NOLINT(google-default-arguments)
{
	m.doStop();
	Q_EMIT stopRequested(flags);
	Q_EMIT stopped();
}

QVector<int> ObjectSensor::getDetectParameters() const
{
	QReadLocker locker(&mDetectParametersLock);
	return mDetectParameters;
}

void ObjectSensor::onResult(const trikDsp::OutArgs &result)
{
	bool hsvUpdated = false;

	if (result.autoDetect) {
		// The DSP tagged this frame as the one-shot detect result. Consume it:
		// clear the pending host flag and feed the detected range back to the
		// DSP, widening it by the config toleranceFactor (as the old worker did
		// before re-sending). A plain local flag can't be used here: frames that
		// were already in flight before detect() reached the DSP would race with
		// the real detect frame.
		m.inArgs().autoDetect = false;
		m.inArgs().params = result.detected;
		applyToleranceFactor(m.inArgs().params, mToleranceFactor);
		hsvUpdated = true;
	}

	{
		QWriteLocker locker(&mReadingLock);
		mReading = {result.location.x, result.location.y,
		            static_cast<int>(result.location.size)};
	}

	if (hsvUpdated) {
		{
			QWriteLocker locker(&mDetectParametersLock);
			mDetectParameters = toDetectParameters(result.detected);
		}
		Q_EMIT activateRequested(m.inArgs(), m.videoOut(), false);
	}
}

