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

#include "event.h"

#include "eventCode.h"

using namespace trikControl;

Event::Event(int eventType)
	: mEventType(eventType)
{
}

EventCodeInterface *Event::code(int codeNum)
{
	if (!mEventCodes.contains(codeNum)) {
		const QSharedPointer<EventCode> eventCode(new EventCode(codeNum));
		connect(this, &Event::on, eventCode.data(), &EventCode::onEvent);
		mEventCodes.insert(codeNum, eventCode);
	}

	return mEventCodes.value(codeNum).data();
}

void Event::onEvent(int event, int code, int value, int eventTime)
{
	if (event == mEventType) {
		emit on(code, value, eventTime);
	}
}
