/* Copyright 2015 CyberTech Labs Ltd.
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

#include <QtCore/QObject>

#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Encoder of power motor.
class TRIKCONTROL_EXPORT EncoderInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public slots:
	/// Returns current encoder reading (in degrees).
	virtual int read() = 0;

	/// Returns current raw reading of encoder.
	virtual int readRawData() = 0;

	/// Resets encoder by setting current reading to 0.
	virtual void reset() = 0;
};

}

Q_DECLARE_METATYPE(trikControl::EncoderInterface *)
