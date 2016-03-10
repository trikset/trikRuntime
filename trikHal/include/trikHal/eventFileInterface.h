/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QtCore/QString>
#include <QtCore/QObject>

namespace trikKernel {
class TimeVal;
}

namespace trikHal {

/// Event file abstraction. Can be opened or closed, when opened can emit signal containing event data.
class EventFileInterface : public QObject
{
	Q_OBJECT

public:
	/// Enum with all event types known to TRIK runtime library. Hides Linux event codes defined in linux/input.h
	/// to be able to work with event file simulators on other operating systems.
	enum class EventType {
		unknown
		, evAbsDistance
		, evAbsMisc
		, evAbsX
		, evAbsY
		, evAbsZ
		, evSyn
		, evKey
	};

	/// Opens event file and starts listening for events.
	/// Note that it will take event from file, so any other application (or other code that works with the same event
	/// file) will not receive it. This may lead to race conditions. One event file shall be open by only one listener
	/// in a given point of time.
	/// @param fileName - file name (with path, relative or absolute) of an event file.
	virtual bool open() = 0;

	/// Closes event file and stops listening for events.
	virtual bool close() = 0;

	/// Abort all pending synchronous event loops.
	virtual void cancelWaiting() = 0;

	/// Returns name of a file.
	virtual QString fileName() const = 0;

	/// Returns true if a file is opened.
	virtual bool isOpened() const = 0;

signals:
	/// Emitted when there is new event in an event file.
	/// @param eventType - type of an event (unknown, if such event is not listed in EventType enum).
	/// @param code - low-level event code.
	/// @param value - low-level event value.
	void newEvent(trikHal::EventFileInterface::EventType eventType, int code, int value
			, const trikKernel::TimeVal &eventTime);
};

}
