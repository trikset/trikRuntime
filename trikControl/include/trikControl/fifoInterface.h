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

#include "declSpec.h"

namespace trikControl {

/// Interface that represents linux FIFO file, which is commonly used by various sensors.
class TRIKCONTROL_EXPORT FifoInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public slots:
	/// Reads data from this FIFO file, returning all available data as string.
	virtual QString read() = 0;

	/// Returns true if FIFO has new data in it.
	virtual bool hasData() const = 0;

signals:
	/// Emitted when new string is arrived to FIFO file. Emitted several times if more than one string arrives at once.
	void newData(const QString &data);
};

}
