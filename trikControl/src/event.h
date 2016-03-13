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
#include <QtCore/QHash>
#include <QtCore/QSharedPointer>

#include "eventInterface.h"
#include "eventCodeInterface.h"

namespace trikControl {

/// Implementation of an event that can be emitted by event device.
class Event : public EventInterface
{
	Q_OBJECT

public:
	/// Consructor.
	/// @param eventType - type of an event to emit. It is a number shown next to the event in evtest output.
	explicit Event(int eventType);

	EventCodeInterface *code(int codeNum) override;

public slots:
	/// Supposed to be called by event device when new event is emitted by event device file. Filters events by type
	/// and re-emits signal.
	void onEvent(int onEvent, int code, int value, int eventTime);

private:
	/// A storage for created event code objects.
	QHash<int, QSharedPointer<EventCodeInterface>> mEventCodes;

	/// Type of events to filter.
	int mEventType = 0;
};

}
