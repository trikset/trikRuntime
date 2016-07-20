/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
 * and CyberTech Labs Ltd.
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

#include "encoderInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class MspCommunicatorInterface;

/// Implementation of encoder for real robot.
class Encoder : public EncoderInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this encoder is configured.
	/// @param configurer - configurer object containing preparsed XML files with encoder parameters.
	/// @param communicator - I2C communicator to use to query encoder.
	Encoder(const QString &port, const trikKernel::Configurer &configurer
			, trikControl::MspCommunicatorInterface &communicator);

	Status status() const override;

public slots:
	int read() override;

	int readRawData() override;

	void reset() override;

private:
	MspCommunicatorInterface &mCommunicator;
	int mI2cCommandNumber;
	int mPassedTicks;
	int mPassedDegrees;
	const bool mInvert;
	DeviceState mState;
};

}
