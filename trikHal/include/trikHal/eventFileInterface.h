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
#include <trikKernel/timeVal.h>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Event file abstraction. Can be opened or closed, when opened can emit signal containing event data.
class TRIKHAL_EXPORT EventFileInterface : public QObject
{
	Q_OBJECT

public:
	/// Opens event file and starts listening for events.
	virtual bool open() = 0;

	/// Closes event file and stops listening for events.
	virtual bool close() = 0;

	/// Abort all pending synchronous event loops.
	virtual void cancelWaiting() = 0;

	/// Returns name of an event file.
	virtual QString fileName() const = 0;

	/// Returns true if a file is opened.
	virtual bool isOpened() const = 0;

signals:
	/// Emitted when there is new event in an event file.
	/// @param eventType - low-level type of an event.
	/// @param code - low-level event code.
	/// @param value - low-level event value.
	void newEvent(int eventType, int code, int value, trikKernel::TimeVal eventTime);
};

}
