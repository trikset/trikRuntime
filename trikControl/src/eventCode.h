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

namespace trikControl {

/// Implementation of event code filter for event devices.
class EventCode : public EventCodeInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param code - code of the event to filter, see evtest output for a list of codes for specific device.
	explicit EventCode(int code);

public Q_SLOTS:
	/// Supposed to be called by event type filter when new event is emitted. Filters events of this
	/// type by code and re-emits signal.
	void onEvent(int code, int value, int eventTime);

private:
	/// Code of event to filter.
	int mEventCode = 0;
};

}
