/* Copyright 2016 CyberTech Labs Ltd.
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

#include "eventCodeInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Represents event that can be produced by event device. This class provides ability to subscribe to specific event
/// of a device, and the ability to subscribe to event with specific code.
class TRIKCONTROL_EXPORT EventInterface : public QObject
{
	Q_OBJECT

public slots:
	/// Returns object that emits only events with specific code. See evtest output for a list of codes for
	/// specific device.
	virtual EventCodeInterface *code(int code) = 0;

signals:
	/// Emitted when there is new event in an event file.
	/// @param code - code of the event inside a type.
	/// @param value - value sent with the event.
	/// @param eventTime - time stamp of the event, in milliseconds from start of the Unix time (modulo 2^32).
	void on(int code, int value, int eventTime);
};

}

Q_DECLARE_METATYPE(trikControl::EventInterface *)
