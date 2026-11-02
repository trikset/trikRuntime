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

#include <QtCore/QByteArray>
#include <QtCore/QVector>

#include "peripheryCommunicatorInterface.h"

namespace trikControl {

/// Interface with I2C-specific commands, adds readX to PeripheryCommunicatorInterface.
class I2cCommunicatorInterface : public PeripheryCommunicatorInterface
{
public:
	/// Reads raw bytes via I2C (used by I2cDevice).
	virtual QVector<uint8_t> readX(const QByteArray &data) = 0;
};

}