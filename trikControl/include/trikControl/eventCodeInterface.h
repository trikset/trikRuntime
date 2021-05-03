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

#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Class that emits events from event device with specific event type and event code. Allows to selectively listen
/// to events.
class TRIKCONTROL_EXPORT EventCodeInterface : public QObject
{
	Q_OBJECT

signals:
	/// Emitted when there is new event with specific type and code in an event file.
	/// @param value - value sent with the event.
	/// @param eventTime - time stamp of the event, in milliseconds from start of the Unix time (modulo 2^32).
	void on(int value, int eventTime);
};

}

Q_DECLARE_METATYPE(trikControl::EventCodeInterface *)
