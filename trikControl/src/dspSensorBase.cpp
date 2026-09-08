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

#include "dspSensorBase.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

namespace trikControl {

DspSensorHelper::DspSensorHelper(const QString &name, const trikKernel::Configurer &configurer,
                                 const QString &port, trikDsp::Algorithm algo) // NOLINT(modernize-pass-by-value)
	: mState(name)
	, mConfigurer(configurer)
	, mPort(port)
	, mAlgo(algo)
{
}

bool DspSensorHelper::doInit(bool showOnDisplay)
{
	if (mState.isFailed()) {
		QLOG_ERROR() << "An attempt to start the sensor in a failed state: " << mPort;
		return false;
	}

	mVideoOut = showOnDisplay;
	return true;
}

void DspSensorHelper::doStop()
{
	mVideoOut = false;
}

}

